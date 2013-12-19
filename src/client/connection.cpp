#include "connection.hpp"

#include <boost/make_shared.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "data_container.hpp"
#include "client_impl.hpp"


connection_t::connection_t(const std::string &connection_string)
    : m_context(1),
      m_address(connection_string)
{
}

boost::shared_ptr<zmq::socket_t> connection_t::create_socket(int type) {
    std::string random_identity =to_string(boost::uuids::random_generator()());
    return create_socket(type, random_identity);
}

boost::shared_ptr<zmq::socket_t> connection_t::create_socket(int type, const std::string &identity) {
    boost::shared_ptr<zmq::socket_t> socket = boost::make_shared<zmq::socket_t>(m_context, type);
    int zero = 0;
    socket->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));
    socket->setsockopt(ZMQ_IDENTITY, (void*)identity.c_str(), identity.size());
    socket->connect(m_address.c_str());
    return socket;
}



sender_ptr_t client_creation_policy<sender_iface_t>::create(const boost::shared_ptr<zmq::socket_t> &socket, const std::string &name) {
    return boost::make_shared<sender_impl_t>(socket, name);
}

receiver_ptr_t client_creation_policy<receiver_iface_t>::create(const boost::shared_ptr<zmq::socket_t> &socket, const std::string &name) {
    return boost::make_shared<receiver_impl_t>(socket, name);
}
