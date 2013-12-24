#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <string>

#include "data_container.hpp"

#include <boost/shared_ptr.hpp>

class connection_t;
typedef boost::shared_ptr<connection_t> connection_ptr_t;

namespace zmq {
    class socket_t;
}

class client_t {
public:
    client_t(const connection_ptr_t& connection)
        : m_connection(connection)
    {
    }

protected:
    boost::shared_ptr<zmq::socket_t> create_socket(int type);
    boost::shared_ptr<zmq::socket_t> create_socket(int type, const std::string& identity);

    static std::string generate_uuid();

    connection_ptr_t m_connection;
};

class sender_iface_t  : public client_t {
public:
    sender_iface_t(const connection_ptr_t& connection)
        : client_t(connection)
    {
    }

    virtual ~sender_iface_t() { }
    void send(const std::string&);
    void send(const std::vector<std::string>&);
private:
    virtual void send(data_container_t&) = 0;
};

typedef boost::shared_ptr<sender_iface_t> sender_ptr_t;

class receiver_iface_t : public client_t {
public:
    receiver_iface_t(const connection_ptr_t& connection)
        : client_t(connection)
    {
    }

    virtual ~receiver_iface_t() { }
    void recv(std::vector<std::string>&);
private:
    virtual void recv(data_container_t&) = 0;
};

typedef boost::shared_ptr<receiver_iface_t> receiver_ptr_t;

class async_receiver_iface_t : public client_t {
public:
    class handler_t {
    public:
        virtual void on_recv(std::vector<std::string>&) = 0;
        virtual void on_disconnect() = 0;
    };

    typedef boost::shared_ptr<handler_t> handler_ptr_t;

    async_receiver_iface_t(connection_ptr_t connection)
        : client_t(connection)
    {
    }

    virtual ~async_receiver_iface_t() { }

    virtual void set_handler(const handler_ptr_t& handler) = 0;
};

typedef boost::shared_ptr<async_receiver_iface_t> async_receiver_ptr_t;

#endif // CLIENT_HPP
