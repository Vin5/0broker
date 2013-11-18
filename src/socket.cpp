#include "socket.hpp"

namespace zbroker {

socket_t::socket_t(const context_t &ctx, int socket_type)
    : zmq::socket_t(*ctx.zmq_context(), socket_type),
      m_type(socket_type)
{
}

void socket_t::connect(const endpoint_t &endpoint) {
    zmq::socket_t::connect(endpoint.connection_string().c_str());
}

void socket_t::bind(const endpoint_t &endpoint) {
    zmq::socket_t::bind(endpoint.connection_string().c_str());
}

bool socket_t::has_more() {
    int64_t more = 1;
    size_t more_size = sizeof (more);
    zmq::socket_t::getsockopt(ZMQ_RCVMORE, &more, &more_size);
    return more;
}

}
