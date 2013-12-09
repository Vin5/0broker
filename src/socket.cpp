#include "socket.hpp"
#include "context.hpp"
#include "endpoint.hpp"
#include "message.hpp"

namespace zbroker {

socket_t::socket_t(const context_t &ctx, int socket_type)
    : m_socket(*ctx.zmq_context(), socket_type),
      m_type(socket_type)
{
}

void socket_t::connect(const endpoint_t &endpoint) {
    m_socket.connect(endpoint.connection_string().c_str());
}

void socket_t::bind(const endpoint_t &endpoint) {
    m_socket.bind(endpoint.connection_string().c_str());
}

bool socket_t::send(zmq::message_t &msg, int flags) {
    return m_socket.send(msg, flags);
}

bool socket_t::recv(zmq::message_t *msg, int flags) {
    return m_socket.recv(msg, flags);
}

bool socket_t::send(message_pack_t &msg, int flags) {
    if(msg.empty()) {
        return false;
    }
    size_t msg_size = msg.size();
    size_t i = 0;
    while((msg_size - i) != 1) {
        message_part_t& part = msg[i++];
        if(send(*part, ZMQ_SNDMORE | flags))
            return false;
    }
    message_part_t& part = msg[i];
    return send(*part, flags);
}

bool socket_t::recv(message_pack_t &msg_pack, int flags) {
    do {
        message_part_t msg(new zmq::message_t);
        if(!m_socket.recv(msg.get(), flags))
            return false;
        msg_pack.push_tail(std::move(msg));
    } while (has_more());
    return true;
}

bool socket_t::has_more() {
    int64_t more = 1;
    size_t more_size = sizeof (more);
    m_socket.getsockopt(ZMQ_RCVMORE, &more, &more_size);
    return more;
}

} // zbroker
