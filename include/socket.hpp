#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "forwards.hpp"
#include "context.hpp"
#include "endpoint.hpp"

#include <set>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace zbroker {

class socket_t : boost::noncopyable {
public:
    socket_t(const context_t& ctx, int socket_type);

    void connect(const endpoint_t& endpoint);
    void bind(const endpoint_t& endpoint);

    template<class T>
    bool send(const T& message, size_t size, int flags = 0) {
        zmq::message_t msg(size);
        memcpy(msg.data(), &message, size);
        return m_socket.send(msg, flags);
    }

    template<class T>
    bool send(const T* message, size_t size, int flags = 0) {
        zmq::message_t msg(size);
        memcpy(msg.data(), message, size);
        return m_socket.send(msg, flags);
    }

    bool send(const std::string& message, int flags = 0) {
        return send(message.data(), message.size(), flags);
    }


    template<class T>
    bool recv(T& message, size_t size, int flags = 0) {
        zmq::message_t msg;
        if(!m_socket.recv(&msg, flags)) {
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
        if(!m_socket.recv(&msg, flags)) {
            return false;
        }
        message.assign(static_cast<const char*>(msg.data()), msg.size());
        return true;
    }

    operator void* () {
        return m_socket;
    }

private:
    zmq::socket_t m_socket;
    int m_type;
};

typedef boost::shared_ptr<socket_t> socket_ptr_t;
typedef std::set<socket_ptr_t> socket_set_t;

} // zbroker

#endif // SOCKET_HPP
