#include "connection.hpp"
#include "client.hpp"
#include <iostream>

#include <unistd.h>

void client_code() {
    try {
        connection_ptr_t connection = connection_t::create("tcp://localhost:5555");
        sender_ptr_t sender = connection->get<sender_t>("randevouz");
        data_list_t sdata;
        sdata.push_back("Hello ");
        sdata.push_back("World!");
        sender->send(sdata);

        receiver_ptr_t receiver = connection->get<receiver_t>("randevouz");
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



class async_handler_t : public async_receiver_t::handler_t {
public:
    async_handler_t()
    {
        count = 0;
    }

    void on_recv(std::vector<std::string> & data) {
        for(unsigned int i = 0; i < data.size(); i++) {
            count++;
            std::cout << count << " " << data[i] << std::endl;
        }
    }
    void on_disconnect() {
        std::cout << "disconnect" << std::endl;
    }
    int count;
};

void client_code2() {
    try {
        connection_ptr_t connection = connection_t::create("tcp://localhost:5555");
        sender_ptr_t sender = connection->get<sender_t>("randevouz");
        for(int i = 0; i < 5; i++) {
            data_list_t sdata;
            sdata.push_back("Hello World!");
            sender->send(sdata);
        }

        async_receiver_ptr_t receiver = connection->get<async_receiver_t>("randevouz");
        async_receiver_t::handler_ptr_t handler(new async_handler_t);
        receiver->set_handler(handler);

        sleep(2);
    }
    catch(std::exception& e) {
        std::cout << e.what();
    }
}

int main(int argc, char* argv[]) {
    client_code2();
}
