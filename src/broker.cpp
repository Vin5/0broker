#include "broker.hpp"
#include "context.hpp"
#include "config.hpp"
#include "socket.hpp"
#include "poller.hpp"
#include "endpoint.hpp"
#include "stl_helpers.hpp"
#include "message.hpp"
#include "errors.hpp"
#include <iostream>
namespace codes {
    namespace header {
        static const char* const sender =   "001";
        static const char* const receiver = "002";
    }

    namespace control {
        namespace sender {
            static const char* const put =          "001";
        }

        namespace receiver {
            static const char* const ready =        "001";
            static const char* const reply =        "002";
            static const char* const heartbeat =    "003";
            static const char* const disconnect =   "004";
        }
    }
}

namespace zbroker {

broker_t::broker_t(const context_ptr_t& ctx)
    : m_ctx(ctx),
      m_interrupted(false)
{
    m_socket.reset(new socket_t(*m_ctx, ZMQ_ROUTER));
    m_socket->bind(m_ctx->config()->address());
}

void broker_t::run() {
    m_ctx->log(LL_INFO) << "Broker has started at " << m_ctx->config()->address();
    try {
        const size_t BROKER_SOCKET_INDEX = 0;
        const size_t HEARTBEAT_INTERVAL = m_ctx->config()->heartbeat_interval();
        m_heartbeat_time = time_point_t() + HEARTBEAT_INTERVAL;

        poller_t poller;
        poller.add(*m_socket);

        while(true) {
            if(!poller.poll_in(HEARTBEAT_INTERVAL)) {
                break; // interrupted or error occured
            }
            if(poller.check(BROKER_SOCKET_INDEX, ZMQ_POLLIN)) {
                if(!handle_request()) {
                    break; // interrupted
                }
            }
            if(time_point_t() > m_heartbeat_time) {
                renew_recipients();
                m_heartbeat_time = time_point_t() + HEARTBEAT_INTERVAL;
            }
        }
        if(m_interrupted) {
            m_ctx->log(LL_INFO) << "Broker has been interrupted by a signal. Stop serving.";
        }
    }
    catch(const std::exception& e) {
        m_ctx->log(LL_ERROR) << e.what();
    }
}

void broker_t::interrupt() {
    m_interrupted = true;
}

bool broker_t::handle_request()  {
    message_pack_t message;
    if(!m_socket->recv(message))
        return false; // interrupted

    message_part_t sender = message.pop_head();
    message_part_t empty = message.pop_head();
    message_part_t header = message.pop_head();

    try {
        if(message::equal_to(*header, codes::header::sender)) {
            handle_sender(message);
        }
        else if(message::equal_to(*header, codes::header::receiver)) {
            message.push_head(sender);
            handle_receiver(message);
        }
        else {
            throw runtime_error_t("%s", "Malformed message");
        }
    }
    catch(const runtime_error_t& e) {
        m_ctx->log(LL_WARNING) << "Message didn't treat properly (" << e.what() << ").";
    }

    return true;
}

void broker_t::handle_sender(message_pack_t& msg) {

    message_part_t command = msg.pop_head();
    if(!message::equal_to(*command, codes::control::sender::put)) {
        throw runtime_error_t("%s", "Malformed message");
    }

    std::string service_name;
    msg.pop_head(service_name);

    // get or create a service
    service_ptr_t service = lookup_service(service_name);

    // the rest of the message is a payload
    service->append_message(std::move(msg));
    service->dispatch(m_socket);
}

broker_t::service_ptr_t broker_t::lookup_service(const std::string &name) {
    auto service_iterator = m_services.find(name);
    if(service_iterator == m_services.end()) {
        service_ptr_t service = boost::make_shared<service_t>(name);
        m_services.insert(std::make_pair(name, service));
        return service;
    }
    return service_iterator->second;
}

void broker_t::handle_receiver(message_pack_t &msg) {
    message_part_t receiver = msg.pop_head();
    std::string address;
    message::unpack(address, *receiver);

    // get or create recipient
    recipient_ptr_t recipient = lookup_recipient(receiver);
    message_part_t command = msg.pop_head();
    if(message::equal_to(*command, codes::control::receiver::ready)) {
        recipient->update_expiration(next_expiration());

        std::string service_name;
        msg.pop_head(service_name);

        // get or create a service and attach recipient
        service_ptr_t service = lookup_service(service_name);
        service->attach_waiter(recipient);
        service->dispatch(m_socket);
    }
    else if(message::equal_to(*command, codes::control::receiver::heartbeat)) {
        recipient->update_expiration(next_expiration());
    }
    else if(message::equal_to(*command, codes::control::receiver::disconnect)) {
        recipient->disconnect();
        m_recipients.erase(address);
    }
    else {
        recipient->disconnect(); // we don't need a problem
        throw runtime_error_t("%s", "Malformed message");
    }
}


broker_t::recipient_ptr_t broker_t::lookup_recipient(const message_part_t &receiver) {
    std::string address;
    message::unpack(address, *receiver);

    auto recipient_iterator = m_recipients.find(address);
    if(recipient_iterator == m_recipients.end()) { // never seen before
        recipient_ptr_t recipient = boost::make_shared<recipient_t>(receiver);
        recipient->update_expiration(next_expiration());
        m_recipients.insert(std::make_pair(address, recipient));
        return recipient;
    }
    return recipient_iterator->second;
}

time_point_t broker_t::next_expiration() {
    const static unsigned int EXPIRATION_INTERVAL = m_ctx->config()->heartbeat_interval() * m_ctx->config()->liveness();
    return time_point_t() + EXPIRATION_INTERVAL;
}

void broker_t::renew_recipients() {
    auto recipient_iterator = m_recipients.begin();
    while(recipient_iterator != m_recipients.end()) {
        recipient_ptr_t& recipient = recipient_iterator->second;
        if(recipient->expired() || recipient->disconnected()) {
            m_recipients.erase(recipient_iterator++);
        }
        else {
            send_heartbeat(recipient);
            ++recipient_iterator;
        }
    }
}

void broker_t::send_heartbeat(const broker_t::recipient_ptr_t &recipient) {

}

void broker_t::service_t::attach_waiter(const broker_t::recipient_ptr_t &waiter) {
    waiting.remove(waiter);
    waiting.push_back(waiter);
}

void broker_t::service_t::append_message(message_pack_t && msg) {
    messages.push_back(msg);
}

void broker_t::service_t::dispatch(const socket_ptr_t &backend) {

    // purge stale recipients
    waiting.remove_if([](const broker_t::recipient_ptr_t& recipient) { return recipient->expired() || recipient->disconnected(); });

    while(!messages.empty() && !waiting.empty()) {
        broker_t::recipient_ptr_t recipient = waiting.front();

        message_pack_t& message = messages.front();

        backend->send(*recipient->identity(), ZMQ_SNDMORE);
        backend->send("", ZMQ_SNDMORE);
        backend->send(message);

        recipient->disconnect(); // disconnect served recipient
        waiting.pop_front();
        messages.pop_front();
    }
}

message_part_t broker_t::recipient_t::identity() const {
    return m_identity;
}

bool broker_t::recipient_t::expired() const {
    return time_point_t() > m_expiry;
}

bool broker_t::recipient_t::disconnected() const {
    return m_disconnected;
}

void broker_t::recipient_t::update_expiration(const time_point_t &expiration) {
    m_expiry = expiration;
}

void broker_t::recipient_t::disconnect() {
    m_disconnected = true;
}


} // zbroker
