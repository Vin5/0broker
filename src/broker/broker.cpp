#include "broker.hpp"
#include "context.hpp"
#include "config.hpp"
#include "socket.hpp"
#include "poller.hpp"
#include "endpoint.hpp"
#include "message.hpp"
#include "errors.hpp"
#include "recipient.hpp"
#include "service.hpp"

#include "codes.hpp"

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
            message.push_head(std::move(sender));
            handle_receiver(message);
        }
        else {
            m_ctx->log(LL_WARNING) << "Message discarded (header part is corrupted)";
        }
    }
    catch(const runtime_error_t& e) {
        m_ctx->log(LL_WARNING) <<  e.what();
    }

    return true;
}

void broker_t::handle_sender(message_pack_t& msg) {

    message_part_t command = msg.pop_head();
    if(!message::equal_to(*command, codes::control::sender::put)) {
        throw runtime_error_t("%s", "Message discarded (command part is corrupted)");
    }

    std::string service_name;
    msg.pop_head(service_name);

    // get or create a service
    service_ptr_t service = lookup_service(service_name);

    // the rest of the message is a payload
    service->append_message(std::move(msg));
    service->dispatch(m_socket);
}

service_ptr_t broker_t::lookup_service(const std::string &name) {
    auto service_iterator = m_services.find(name);
    if(service_iterator == m_services.end()) {
        service_ptr_t service = std::make_shared<service_t>(name);
        m_services.insert(std::make_pair(name, service));
        return service;
    }
    return service_iterator->second;
}

void broker_t::handle_receiver(message_pack_t &msg) {
    std::string receiver;
    msg.pop_head(receiver);

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
        m_recipients.erase(receiver);
    }
    else {
        recipient->disconnect(); // we don't need a problem
        throw runtime_error_t("%s", "Message discarded (command part is corrupted)");
    }
}


recipient_ptr_t broker_t::lookup_recipient(const std::string& address) {
    auto recipient_iterator = m_recipients.find(address);
    if(recipient_iterator == m_recipients.end()) { // never seen before
        recipient_ptr_t recipient = std::make_shared<recipient_t>(address);
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

void broker_t::send_heartbeat(const recipient_ptr_t &recipient) {
    m_socket->send(recipient->identity(), ZMQ_SNDMORE);
    m_socket->send("", ZMQ_SNDMORE);
    m_socket->send(codes::control::broker::heartbeat);
}

} // zbroker
