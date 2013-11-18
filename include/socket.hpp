#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "forwards.hpp"
#include "context.hpp"
#include "endpoint.hpp"

#include <stdexcept>
#include <set>

#include <zmq.hpp>

#include <boost/shared_ptr.hpp>

namespace zbroker {

class socket_t : zmq::socket_t {
public:

    using zmq::socket_t::operator void *;
    using zmq::socket_t::recv;
    using zmq::socket_t::send;

    socket_t(const context_t& ctx, int socket_type);

    void connect(const endpoint_t& endpoint);
    void bind(const endpoint_t& endpoint);

    template<class T>
    bool send(const T& message, size_t size, int flags = 0) {
        zmq::message_t msg(size);
        memcpy(msg.data(), &message, size);
        return zmq::socket_t::send(msg, flags);
    }

    template<class T>
    bool send(const T* message, size_t size, int flags = 0) {
        zmq::message_t msg(size);
        memcpy(msg.data(), message, size);
        return zmq::socket_t::send(msg, flags);
    }

    bool send(const std::string& message, int flags = 0) {
        return send(message.data(), message.size(), flags);
    }


    template<class T>
    bool recv(T& message, size_t size, int flags = 0) {
        zmq::message_t msg;
        if(!zmq::socket_t::recv(&msg, flags)) {
            return false;
        }
        if(size != msg.size()) {
            std::string err = "Message size is not equal to destination object size";
            throw std::runtime_error(err);
        }
        memcpy(&message, msg.data(), size);
        return true;
    }

    template<class T>
    bool recv(T* message, size_t size, int flags = 0) {
        assert(message);
        return recv(*message, size, flags);
    }

    bool recv(std::string& message, int flags = 0) {
        zmq::message_t msg;
        if(!zmq::socket_t::recv(&msg, flags)) {
            return false;
        }
        message.assign(static_cast<const char*>(msg.data()), msg.size());
        return true;
    }

    bool has_more();

private:
    int m_type;
};

typedef boost::shared_ptr<socket_t> socket_ptr_t;

} // zbroker

#endif // SOCKET_HPP
