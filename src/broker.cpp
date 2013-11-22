#include "broker.hpp"

#include "socket.hpp"
#include "poller.hpp"
#include "endpoint.hpp"
#include "stl_helpers.hpp"

namespace zbroker {

broker_t::broker_t(const context_ptr_t& ctx)
    : m_ctx(ctx)
{
}

void broker_t::run() {
    try {
        endpoint_t frontend_address(TT_IPC, "/tmp/frontend.ipc");
        endpoint_t backend_address(TT_IPC, "/tmp/backend.ipc");

        socket_t frontend(*m_ctx, ZMQ_ROUTER);
        frontend.bind(frontend_address);

        socket_t backend(*m_ctx, ZMQ_ROUTER);
        backend.bind(backend_address);

        const size_t FRONTEND = 0;
        const size_t BACKEND = 1;

        poller_t poller;

        poller.add(frontend);
        poller.add(backend);

        while(true) {
            const int TIMEOUT = 10000;
            if(!poller.poll_in(TIMEOUT)) {
                // LOG poll failed
                break;
            }
            if(poller.check(FRONTEND, ZMQ_POLLIN)) {
                if(!handle_frontend(frontend)) {
                    break;
                }
            }
            if(poller.check(BACKEND, ZMQ_POLLIN)) {
                if(!handle_backend(backend)) {
                    break;
                }
            }
            // detect pending messages and send them to receivers
            if(!distribute_messages(backend)) {
                break;
            }
        }
    }
    catch(const std::exception& e) {
        m_ctx->log(LL_ERROR) << e.what();
    }
}

bool broker_t::handle_frontend(socket_t &frontend)  {
    std::string sender_address;
    if(!frontend.recv(sender_address))
        return false;

    std::string empty_part;
    if(!frontend.recv(empty_part))
        return false;

    std::string destination;
    if(!frontend.recv(destination))
        return false;

    msg_pack_t msg_pack;
    do {
        msg_ptr_t msg(new zmq::message_t);
        if(!frontend.recv(msg.get()))
            return false;
        msg_pack.push_back(msg);
    } while (frontend.has_more());

    append_pending_messages(destination, std::move(msg_pack));

    // acknowledgemt message
    frontend.send(sender_address, ZMQ_SNDMORE);
    frontend.send(std::string(), ZMQ_SNDMORE);
    frontend.send(std::string("OK"));

    return true;
}

void broker_t::append_pending_messages(const std::string& destination, msg_pack_t&& messages) {
    auto pending_messages_iterator = m_pending_messages.find(destination);
    if(pending_messages_iterator != m_pending_messages.end()) {
        auto& pending_container = pending_messages_iterator->second;
        pending_container.push_back(messages);
    }
    else {
        std::deque<msg_pack_t> pending_container;
        pending_container.push_back(messages);
        m_pending_messages.insert(std::make_pair(destination, pending_container));
    }
}

bool broker_t::handle_backend(socket_t& backend) {
    std::string consumer_address;
    if(!backend.recv(consumer_address))
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

void broker_t::append_consumer(const std::string& destination, std::string&& consumer_address) {
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

bool broker_t::send_msg_pack(socket_t &backend, const std::string &address, const broker_t::msg_pack_t &messages) {
    backend.send(address, ZMQ_SNDMORE);
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
}


} // zbroker
