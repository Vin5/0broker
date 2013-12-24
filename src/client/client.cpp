#include "client.hpp"
#include "data_container.hpp"
#include "connection.hpp"


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

void sender_iface_t::send(const std::string &data) {
    data_container_t container(data);
    send(container);
}

void sender_iface_t::send(const std::vector<std::string> &data) {
    data_container_t container(data);
    send(container);
}


void receiver_iface_t::recv(std::vector<std::string>& data) {
    data_container_t container;
    recv(container);
    container.get_data(data);
}


boost::shared_ptr<zmq::socket_t> client_t::create_socket(int type) {
    return m_connection->create_socket(type);
}

boost::shared_ptr<zmq::socket_t> client_t::create_socket(int type, const std::string &identity) {
    return m_connection->create_socket(type, identity);
}

std::string client_t::generate_uuid() {
    return to_string(boost::uuids::random_generator()());
}
