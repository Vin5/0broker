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

// common ancestor for every client
// provides an api to connection specific methods
class client_t {
public:
    // broker's response waiting time
    void set_timeout(unsigned int milliseconds);
    unsigned int timeout() const;

    // connection to broker is considered lost after 'retries' unresponsive requests
    void set_retries_count(unsigned int retries);
    unsigned int retries_count() const;

protected:
    explicit client_t(const connection_ptr_t& connection)
        : m_connection(connection),
          m_timeout(2500),
          m_retries(3)
    {
    }

    // broker address
    const std::string& address() const;

    // create socket with specified type
    boost::shared_ptr<zmq::socket_t> create_socket(int type);

    // create socket with specified type and identity
    boost::shared_ptr<zmq::socket_t> create_socket(int type, const std::string& identity);

    // generate unique string identifier
    static std::string generate_uuid();

private:
    connection_ptr_t m_connection;

    unsigned int m_timeout;
    unsigned int m_retries;
};

// interface for data sending tasks
class sender_t  : public client_t {
public:
    explicit sender_t(const connection_ptr_t& connection)
        : client_t(connection)
    {
    }

    virtual ~sender_t() { }
    void send(const std::string&);
    void send(const std::vector<std::string>&);
private:
    virtual void send(data_container_t&) = 0;
};

typedef boost::shared_ptr<sender_t> sender_ptr_t;

// synchronous interface for data receiving tasks
class receiver_t : public client_t {
public:
    explicit receiver_t(const connection_ptr_t& connection)
        : client_t(connection)
    {
    }

    virtual ~receiver_t() { }

    // blocks till data is received or broker stops responding to requests
    void recv(std::vector<std::string>&);
private:
    virtual void recv(data_container_t&) = 0;
};

typedef boost::shared_ptr<receiver_t> receiver_ptr_t;


// asynchronous interface for data receiving tasks
class async_receiver_t : public client_t {
public:
    // asynchronous callback
    class handler_t {
    public:
        // on_recv is called if data is received
        virtual void on_recv(std::vector<std::string>&) = 0;

        // on_disconnect is called if there is not connection to the broker
        virtual void on_disconnect() = 0;
    };

    typedef boost::shared_ptr<handler_t> handler_ptr_t;

    explicit async_receiver_t(connection_ptr_t connection)
        : client_t(connection)
    {
    }

    virtual ~async_receiver_t() { }

    // set async events callback
    virtual void set_handler(const handler_ptr_t& handler) = 0;
};

typedef boost::shared_ptr<async_receiver_t> async_receiver_ptr_t;

#endif // CLIENT_HPP
