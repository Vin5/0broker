#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "forwards.hpp"
#include "msg_traits.hpp"

namespace zbroker {

class socket_t {
public:

    socket_t(const context_t& ctx, int socket_type);

    void connect(const endpoint_t& endpoint);
    void bind(const endpoint_t& endpoint);

    bool send (zmq::message_t &msg, int flags = 0);
    bool send (message_pack_t &msg, int flags = 0);

    template<class T>
    inline bool send(const T& data, int flags = 0) {
        zmq::message_t message;
        message_traits<T>::pack(message, data);
        return send(message, flags);
    }

    bool recv (zmq::message_t *msg, int flags = 0);
    bool recv (message_pack_t& msg, int flags = 0);

    template<class T>
    inline bool recv(T& data, int flags = 0) {
        zmq::message_t message;
        if(!recv(&message, flags)) {
            return false;
        }
        message_traits<T>::unpack(data, message);
        return true;
    }



    bool has_more();

    inline operator void*() {
        return m_socket;
    }

private:
    zmq::socket_t m_socket;
    int m_type;

    socket_t(socket_t&);
    void operator=(socket_t&);
};



} // zbroker

#endif // SOCKET_HPP
