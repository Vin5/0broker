#include "broker.hpp"

#include "config.hpp"
#include "socket.hpp"
#include "poller.hpp"
#include "endpoint.hpp"
#include "stl_helpers.hpp"
#include "message.hpp"

#include <boost/make_shared.hpp>


namespace codes {
    namespace header {
        static const char* const sender =   "001";
        static const char* const reciever = "002";
    }

    namespace control {
        namespace sender {
            static const char* const put =          "001";
        }

        namespace reciever {
            static const char* const get =          "001";
            static const char* const reply =        "002";
            static const char* const heartbeat =    "003";
            static const char* const disconnect =   "004";
        }
    }
}

namespace zbroker {

broker_t::broker_t(const context_ptr_t& ctx)
    : m_ctx(ctx)
{
    m_socket.reset(new socket_t(*m_ctx, ZMQ_ROUTER));
    m_socket->bind(m_ctx->config()->address());
}

void broker_t::run() {
    m_ctx->log(LL_INFO) << "Broker has started at " << m_ctx->config()->address();
    try {
        const size_t BROKER_SOCKET = 0;
        const size_t HEARTBEAT_INTERVAL = m_ctx->config()->heartbeat_interval();

        poller_t poller;
        poller.add(*m_socket);

        while(true) {

            if(!poller.poll_in(HEARTBEAT_INTERVAL)) {
                // LOG poll failed
                break;
            }
            if(poller.check(BROKER_SOCKET, ZMQ_POLLIN)) {
                if(!handle_request()) {
                    break;
                }
            }
            // detect pending messages and send them to receivers
            /*if(!distribute_messages(backend)) {
                break;
            }*/
        }
    }
    catch(const std::exception& e) {
        m_ctx->log(LL_ERROR) << e.what();
    }
}

bool broker_t::handle_request()  {
    zmq::message_t sender;
    if(!m_socket->recv(&sender))
        return false;

    zmq::message_t empty_part;
    if(!m_socket->recv(&empty_part))
        return false;

    zmq::message_t header;
    if(!m_socket->recv(&header))
        return false;

    const char* status_code;

    if(message::equal_to(header, codes::header::sender)) {
        if(!handle_sender(sender)) {
            status_code = "201";
        }
        else {
            status_code = "404";
        }
    }
    else if(message::equal_to(header, codes::header::reciever)) {
        if(!handle_reciever(sender)) {
            status_code = "201";
        }
        else {
            status_code = "404";
        }
    }
    else {
        m_ctx->log(LL_WARNING) << "Recieved malformed message";
        status_code = "501";
    }

    // acknowledgement message
    m_socket->send(sender, ZMQ_SNDMORE);
    m_socket->send(empty_part, ZMQ_SNDMORE);
    m_socket->send(status_code);

    return true;
}

bool broker_t::handle_sender(zmq::message_t& sender) {
    zmq::message_t service_part;
    if(!m_socket->recv(&service_part))
        return false;

    std::string service_name;
    message::unpack(service_name, service_part);
    service_ptr_t service = lookup_service(service_name);

    // recieve message payload
    message_pack_t payload;
    if(!m_socket->recv(payload))
        return false;

    service->dispatch(m_socket, std::move(payload));
    return true;
}

bool broker_t::handle_reciever(zmq::message_t &reciever) {
    return false;
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

void broker_t::service_t::dispatch(const socket_ptr_t &backend, message_pack_t&& msg) {
    messages.push_back(msg);

    while(!messages.empty() && !waiting.empty()) {
        broker_t::recipient_t& recipient = waiting.front();
        message_pack_t& message = messages.front();

        backend->send(*recipient.identity);
        backend->send("");
        backend->send(message);


        waiting.pop_front();
        messages.pop_front();
    }
}


} // zbroker
