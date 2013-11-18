#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "forwards.hpp"
#include "context.hpp"
#include "endpoint.hpp"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace zbroker {

class socket_t : boost::noncopyable {
public:
    socket_t(const context_t& ctx, int socket_type);

    void connect(const endpoint_t& endpoint);
    void bind(const endpoint_t& endpoint);

private:
    zmq::socket_t m_socket;
    int m_type;
};

} // zbroker

#endif // SOCKET_HPP
