#include "connection.hpp"

#include <boost/make_shared.hpp>

#include "data_container.hpp"
#include "client_impl.hpp"


connection_ptr_t connection_t::create(const std::string &broker_address) {
    return connection_ptr_t(new connection_t(broker_address));
}

const std::string &connection_t::address() const {
    return m_address;
}

connection_t::connection_t(const std::string &connection_string)
    : m_context(1),
      m_address(connection_string)
{
}

boost::shared_ptr<zmq::socket_t> connection_t::create_socket(int type) {
    return create_socket(type, std::string());
}

boost::shared_ptr<zmq::socket_t> connection_t::create_socket(int type, const std::string &identity) {
    boost::shared_ptr<zmq::socket_t> socket = boost::make_shared<zmq::socket_t>(m_context, type);
    int zero = 0;
    socket->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
    if(!identity.empty()) {
        socket->setsockopt(ZMQ_IDENTITY, (void*)identity.c_str(), identity.size());
    }
    return socket;
}



sender_ptr_t client_creation_policy<sender_t>::create(const connection_ptr_t& connection, const std::string &name) {
    return boost::make_shared<sender_impl_t>(connection, name);
}

receiver_ptr_t client_creation_policy<receiver_t>::create(const connection_ptr_t& connection, const std::string &name) {
    return boost::make_shared<receiver_impl_t>(connection, name);
}


async_receiver_ptr_t client_creation_policy<async_receiver_t>::create(const connection_ptr_t &connection, const std::string &name) {
    return boost::make_shared<async_receiver_impl_t>(connection, name);
}
