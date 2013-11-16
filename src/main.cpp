#include <iostream>

#include <vector>
#include <map>
#include <string>
#include <deque>

#include <zmq.hpp>
#include <boost/shared_ptr.hpp>



static void send_msg(zmq::socket_t& sock, const std::string& str, int flags = 0) {
    zmq::message_t msg(str.size());
    memcpy(msg.data(), str.data(), str.size());
    sock.send(msg, flags);
}

static bool recv_msg(zmq::socket_t& sock, std::string& str, int flags = 0) {
    zmq::message_t msg;
    bool rc = sock.recv(&msg, flags);
    if(rc) {
        str.assign(static_cast<const char*>(msg.data()), msg.size());
    }
    return rc;
}


typedef std::vector<std::string> addresses_t;
typedef std::map<std::string, addresses_t> consumers_t;

typedef boost::shared_ptr<zmq::message_t> msg_ptr_t;
typedef std::vector<msg_ptr_t> msg_pack_t;
typedef std::map<std::string, std::deque<msg_pack_t> > pending_messages_t;

consumers_t consumers;
pending_messages_t pending;

static bool send_msg_pack(zmq::socket_t& backend, const std::string& address, const msg_pack_t& messages) {
    send_msg(backend, address, ZMQ_SNDMORE);
    send_msg(backend, "", ZMQ_SNDMORE);
    msg_pack_t::const_iterator next_message = messages.begin();
    size_t left_to_send = messages.size();
    while(next_message != messages.end() && left_to_send > 1) {
        msg_ptr_t msg_part = *next_message;
        backend.send(*msg_part, ZMQ_SNDMORE);
        left_to_send--;
        ++next_message;
    }
    msg_ptr_t last_part = *next_message;
    backend.send(*last_part);
    return true;
}

static bool can_send(addresses_t::iterator& address, addresses_t& consumer_addresses, const std::deque<msg_pack_t>& pending_messages) {
    return (address != consumer_addresses.end()) && !pending_messages.empty();
}

// error prone version
void broker_prototype_1() {
    zmq::context_t ctx(1);
    const char* frontend_endpoint = "ipc:///tmp/frontend.ipc";
    const char* backend_endpoint = "ipc:///tmp/backend.ipc";

    zmq::socket_t frontend(ctx, ZMQ_ROUTER);
    frontend.bind(frontend_endpoint);

    zmq::socket_t backend(ctx, ZMQ_ROUTER);
    backend.bind(backend_endpoint);

    const int FRONTEND = 0;
    const int BACKEND = 1;

    zmq::pollitem_t poll_items[] =
    {
        {frontend, 0, ZMQ_POLLIN, 0},
        {backend, 0, ZMQ_POLLIN, 0}
    };

    while(true) {
        int rc = zmq_poll(poll_items, 2, 1000);
        if(rc < 0)
            break;
        if(poll_items[FRONTEND].revents & ZMQ_POLLIN) {
            std::string sender_address;
            if(!recv_msg(frontend, sender_address))
                break;

            std::string empty_part;
            if(!recv_msg(frontend, empty_part))
                break;

            std::string destination;
            if(!recv_msg(frontend, destination))
                break;

            msg_pack_t msg_pack;
            while(true) {
                msg_ptr_t msg(new zmq::message_t);

                if(!frontend.recv(msg.get()))
                    break;
                msg_pack.push_back(msg);

                int64_t more = 1;
                size_t more_size = sizeof (more);
                frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                if (!more)
                    break; // Last message part
            }
            pending_messages_t::iterator pending_messages_iterator = pending.find(destination);
            if(pending_messages_iterator != pending.end()) {
                std::deque<msg_pack_t>& pending_messages = pending_messages_iterator->second;
                pending_messages.push_back(std::move(msg_pack));
            }
            else {
                std::deque<msg_pack_t> pending_messages;
                pending_messages.push_back(std::move(msg_pack));
                pending.insert(std::make_pair(destination, pending_messages));
            }
        }
        if(poll_items[BACKEND].revents & ZMQ_POLLIN) {
            std::string receiver_address;
            recv_msg(backend, receiver_address);

            std::string empty_part;
            recv_msg(backend, empty_part);

            std::string destination;
            recv_msg(backend, destination);

            consumers_t::iterator consumers_iterator = consumers.find(destination);
            if(consumers_iterator == consumers.end()) {
                addresses_t addresses;
                addresses.push_back(receiver_address);
                consumers.insert(std::make_pair(destination, addresses));
            }
            else {
                addresses_t& addresses = consumers_iterator->second;
                if(addresses.end() == std::find(addresses.begin(), addresses.end(), receiver_address)) {
                    addresses.push_back(receiver_address);
                }
            }

        }
        // detect pending messages and send them to receivers
        pending_messages_t::iterator messages_iterator = pending.begin();
        for(;messages_iterator != pending.end(); ++messages_iterator) {
            const std::string& destination = messages_iterator->first;
            std::deque<msg_pack_t>& pending_messages = messages_iterator->second;
            if(pending_messages.empty()) {
                continue;
            }
            consumers_t::iterator consumer_iterator = consumers.find(destination);
            if(consumer_iterator == consumers.end()) {
                continue;
            }
            addresses_t& consumer_addresses = consumer_iterator->second;
            if(consumer_addresses.empty()) {
               continue;
            }
            // send pending messages to the first ready recipient
            addresses_t::iterator next_address = consumer_addresses.begin();
            for(;can_send(next_address, consumer_addresses, pending_messages); ++next_address) {
                msg_pack_t& messages = pending_messages.front();
                if(!send_msg_pack(backend, *next_address, messages)) {
                    continue;
                }
                pending_messages.pop_front();
            }
        }
    }
}

int main(int argc, char* argv[]) {
    broker_prototype_1();
    return 0;
}
