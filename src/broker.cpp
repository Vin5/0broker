#include "broker.hpp"

#include "config.hpp"
#include "socket.hpp"
#include "poller.hpp"
#include "endpoint.hpp"
#include "stl_helpers.hpp"
#include "message.hpp"

#include <boost/make_shared.hpp>

#define SENDER_HEADER "001"
#define RECIEVER_HEADER "002"

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

    if(message::equal_to(header, SENDER_HEADER)) {
        handle_sender(sender);
    }
    else if(message::equal_to(header, RECIEVER_HEADER)) {
        handle_reciever(sender);
    }
    else {
        // acknowledgement message
        m_socket->send(sender, ZMQ_SNDMORE);
        m_socket->send(empty_part, ZMQ_SNDMORE);
        m_socket->send("501");
        m_ctx->log(LL_WARNING) << "Recieved malformed message";
    }



    //append_pending_messages("test", std::move(msg_pack));



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
/*
void broker_t::append_pending_messages(const std::string& destination, msg_pack_t&& messages) {
    auto pending_messages_iterator = m_pending_messages.find(destination);
    if(pending_messages_iterator != m_pending_messages.end()) {
        auto& pending_container = pending_messages_iterator->second;
        pending_container.push_back(messages);
    }
    else {
        std::list<msg_pack_t> pending_container;
        pending_container.push_back(messages);
        m_pending_messages.insert(std::make_pair(destination, pending_container));
    }
}

bool broker_t::handle_backend(socket_t& backend) {
    msg_ptr_t consumer_address(new zmq::message_t);
    if(!backend.recv(consumer_address.get()))
        return false;

    std::string empty_part;
    if(!backend.recv(empty_part))
        return false;

    std::string destination;
    if(!backend.recv(destination))
        return false;

    append_consumer(destination, std::move(consumer_address));
    return true;
}

void broker_t::append_consumer(const std::string& destination, msg_ptr_t consumer_address) {
    auto consumers_iterator = m_consumers.find(destination);
    if(consumers_iterator == m_consumers.end()) {
        addresses_t addresses;
        addresses.insert(consumer_address);
        m_consumers.insert(std::make_pair(destination, addresses));
    }
    else {
        addresses_t& addresses = consumers_iterator->second;
        if(!stl::contains(addresses, consumer_address)){
            addresses.insert(consumer_address);
        }
    }
}

template<class Iterator, class Container>
static bool can_send(Iterator& address_iterator, const Iterator& address_end, const Container& pending_messages) {
    return (address_iterator != address_end) && !pending_messages.empty();
}

bool broker_t::distribute_messages(socket_t& backend) {

    auto messages_iterator = m_pending_messages.begin();
    for(;messages_iterator != m_pending_messages.end(); ++messages_iterator) {
        const auto& destination = messages_iterator->first;
        auto& pending_messages = messages_iterator->second;
        if(pending_messages.empty()) {
            continue;
        }
        auto consumer_iterator = m_consumers.find(destination);
        if(consumer_iterator == m_consumers.end()) {
            continue;
        }
        addresses_t& consumer_addresses = consumer_iterator->second;
        if(consumer_addresses.empty()) {
           continue;
        }
        // send pending messages to the first ready recipient
        addresses_t::iterator next_address = consumer_addresses.begin();
        for(;can_send(next_address, consumer_addresses.end(), pending_messages); ++next_address) {
            msg_pack_t& messages = pending_messages.front();
            if(!send_msg_pack(backend, *next_address, messages)) {
                continue;
            }
            pending_messages.pop_front();
        }
    }
    return true;
}

bool broker_t::send_msg_pack(socket_t &backend, const msg_ptr_t &address, const broker_t::msg_pack_t &messages) {
    backend.send(*address, ZMQ_SNDMORE);
    backend.send(std::string(), ZMQ_SNDMORE);

    msg_pack_t::const_iterator next_message = messages.begin();
    size_t left_to_send = messages.size();
    while(next_message != messages.end() && left_to_send > 1) {
        msg_ptr_t msg_part = *next_message;
        backend.send(*msg_part, ZMQ_SNDMORE);
        left_to_send--;
        ++next_message;
    }
    msg_ptr_t last_part = *next_message;
    backend.send(*last_part);
    return true;
}*/

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
