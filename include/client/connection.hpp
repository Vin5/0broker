#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <vector>

#include <zmq.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "client.hpp"

class connection_t : public boost::enable_shared_from_this<connection_t> {
public:

    static connection_ptr_t create(const std::string& broker_address);

    template<class Client>
    boost::shared_ptr<Client> get(const std::string& service);

    const std::string& address() const;

private:
    explicit connection_t(const std::string& connection_string);

    boost::shared_ptr<zmq::socket_t> create_socket(int type);
    boost::shared_ptr<zmq::socket_t> create_socket(int type, const std::string& identity);

private:
    friend class client_t;

    zmq::context_t m_context;
    std::string m_address;


    // noncopyable
    connection_t(const connection_t&);
    void operator=(const connection_t&);
};

template<class Client>
struct client_creation_policy;

template<>
struct client_creation_policy<sender_iface_t> {
    static sender_ptr_t create(const connection_ptr_t& connection, const std::string& name);
};

template<>
struct client_creation_policy<receiver_iface_t> {
    static receiver_ptr_t create(const connection_ptr_t& connection, const std::string& name);
};

template<>
struct client_creation_policy<async_receiver_iface_t> {
    static async_receiver_ptr_t create(const connection_ptr_t& connection, const std::string& name);
};

template<class Client>
boost::shared_ptr<Client> connection_t::get(const std::string& service) {
    return client_creation_policy<Client>::create(shared_from_this(), service);
}


#endif // CONNECTION_HPP
