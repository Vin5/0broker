#ifndef CLIENT_IMPL_HPP
#define CLIENT_IMPL_HPP

#include "client.hpp"

#include <zmq.hpp>
#include <string>
#include <boost/shared_ptr.hpp>

class sender_impl_t : public sender_iface_t {
public:
    sender_impl_t(const boost::shared_ptr<zmq::socket_t>& m_socket, const std::string& service);

private:
    virtual void send(data_container_t& data);

    std::string m_service;
    boost::shared_ptr<zmq::socket_t> m_socket;
};

class receiver_impl_t : public receiver_iface_t {
public:
    receiver_impl_t(const boost::shared_ptr<zmq::socket_t>& m_socket, const std::string& service);

private:
    virtual void recv(data_container_t&);

    void send_header();
    void send_command(const std::string& command, int options = 0);
    void send_readiness();

    std::string m_service;
    boost::shared_ptr<zmq::socket_t> m_socket;
};




#endif // CLIENT_IMPL_HPP



