#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <vector>

#include <zmq.hpp>

#include <boost/shared_ptr.hpp>

#include "client.hpp"

class connection_t {
public:

    explicit connection_t(const std::string& connection_string);

    template<class Client>
    boost::shared_ptr<Client> get(const std::string& service);


private:
    boost::shared_ptr<zmq::socket_t> create_socket(int type);
    boost::shared_ptr<zmq::socket_t> create_socket(int type, const std::string& identity);

private:

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
    static sender_ptr_t create(const boost::shared_ptr<zmq::socket_t>& socket, const std::string& name);
};

template<>
struct client_creation_policy<receiver_iface_t> {
    static receiver_ptr_t create(const boost::shared_ptr<zmq::socket_t>& socket, const std::string& name);
};

template<class Client>
boost::shared_ptr<Client> connection_t::get(const std::string& service) {
    boost::shared_ptr<zmq::socket_t> sock = create_socket(ZMQ_DEALER);
    return client_creation_policy<Client>::create(sock, service);
}


#endif // CONNECTION_HPP
