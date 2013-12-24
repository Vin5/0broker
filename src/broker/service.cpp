#include "service.hpp"
#include "recipient.hpp"
#include "socket.hpp"
#include "codes.hpp"
#include <iostream>
namespace zbroker {

void service_t::attach_waiter(const recipient_ptr_t &waiter) {
    waiting.remove(waiter);
    waiting.push_back(waiter);
}

void service_t::append_message(message_pack_t && msg) {
    messages.push_back(std::move(msg));
    std::cout << "messages size: " << messages.size() << std::endl;
}

void service_t::dispatch(const socket_ptr_t &backend) {
    // purge stale recipients
    waiting.remove_if([](const recipient_ptr_t& recipient) { return recipient->expired() || recipient->disconnected(); });

    while(!messages.empty() && !waiting.empty()) {
        recipient_ptr_t recipient = waiting.front();

        message_pack_t& message = messages.front();

        send_message(backend, recipient, message);

        recipient->disconnect(); // disconnect served recipient
        waiting.pop_front();
        messages.pop_front();
}
}

void service_t::send_message(const socket_ptr_t &backend, const recipient_ptr_t &recipient, message_pack_t &msg) {
    std::cout << "Message sent to: " << recipient->identity() << std::endl;
    backend->send(recipient->identity(), ZMQ_SNDMORE);
    backend->send("", ZMQ_SNDMORE);
    backend->send(codes::control::broker::data, ZMQ_SNDMORE);
    backend->send(msg);
}

} // zbroker
