#include "client.hpp"
#include "data_container.hpp"
#include "connection.hpp"


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

void sender_t::send(const std::string &data) {
    data_container_t container(data);
    send(container);
}

void sender_t::send(const std::vector<std::string> &data) {
    data_container_t container(data);
    send(container);
}


void receiver_t::recv(std::vector<std::string>& data) {
    data_container_t container;
    recv(container);
    container.get_data(data);
}


const std::string &client_t::address() const {
    return m_connection->address();
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


void client_t::set_timeout(unsigned int milliseconds) {
    m_timeout = milliseconds;
}

void client_t::set_retries_count(unsigned int retries) {
    m_retries = retries;
}

unsigned int client_t::timeout() const {
    return m_timeout;
}

unsigned int client_t::retries_count() const {
    return m_retries;
}
