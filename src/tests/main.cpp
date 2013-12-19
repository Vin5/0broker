#include "connection.hpp"
#include "client.hpp"
#include <iostream>

void client_code() {
    try {
        connection_t connection("tcp://localhost:5555");
        sender_ptr_t sender = connection.get<sender_iface_t>("randevouz");
        data_list_t sdata;
        sdata.push_back("Hello ");
        sdata.push_back("World!");
        sender->send(sdata);

        receiver_ptr_t receiver = connection.get<receiver_iface_t>("randevouz");
        data_list_t data;
        receiver->recv(data);
        for(unsigned int i = 0; i < data.size(); i++) {
            std::cout << data[i] << std::endl;
        }
    }
    catch(std::exception& e) {
        std::cout << e.what();
    }
}

int main(int argc, char* argv[]) {
    client_code();
}
