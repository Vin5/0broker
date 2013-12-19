#include "client.hpp"
#include "data_container.hpp"


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
