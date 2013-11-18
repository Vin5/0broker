#include "socket.hpp"

#include <zmq.hpp>

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

}
