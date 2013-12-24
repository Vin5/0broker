#ifndef CLIENT_IMPL_HPP
#define CLIENT_IMPL_HPP

#include "client.hpp"

#include <zmq.hpp>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class sender_impl_t : public sender_iface_t {
public:
    sender_impl_t(const connection_ptr_t& connection, const std::string& service);

private:
    virtual void send(data_container_t& data);

    std::string m_service;
    boost::shared_ptr<zmq::socket_t> m_socket;
};

class receiver_impl_t : public receiver_iface_t {
public:
    receiver_impl_t(const connection_ptr_t& connection, const std::string& service);

private:
    virtual void recv(data_container_t&);

    std::string m_service;
    boost::shared_ptr<zmq::socket_t> m_socket;
};

class async_receiver_impl_t : public async_receiver_iface_t {
public:
    ~async_receiver_impl_t();
    async_receiver_impl_t(const connection_ptr_t& connection, const std::string& service);

    virtual void set_handler(const handler_ptr_t& handler);

private:
    void background_receiver();

private:
    handler_ptr_t m_handler;
    std::string m_service;
    boost::shared_ptr<zmq::socket_t> m_background_manager;
};


#endif // CLIENT_IMPL_HPP



