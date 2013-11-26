#ifndef BROKER_HPP
#define BROKER_HPP


#include <set>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <list>

#include "forwards.hpp"
#include "context.hpp"
#include "socket.hpp"
#include <ctime>

namespace zbroker {



class broker_t {


    typedef boost::shared_ptr<zmq::message_t> msg_ptr_t;

    typedef std::set<msg_ptr_t> addresses_t;
    typedef std::map<std::string, addresses_t> consumers_t;


    struct recipient_t {
        msg_ptr_t identity;
        time_t expiry;
    };

    struct service_t {
        explicit service_t(const std::string& name)
            : name (name)
        {
        }

        void dispatch(const socket_ptr_t& backend, message_pack_t&& message);

        std::string name;
        std::deque<message_pack_t> messages;
        std::deque<recipient_t> waiting;
    };

    typedef boost::shared_ptr<service_t> service_ptr_t;
    typedef std::map<std::string, service_ptr_t> service_map_t;


public:
    broker_t(const context_ptr_t& ctx);

    void run();

private:
    bool handle_request();
    bool handle_sender(zmq::message_t &sender);
    bool handle_reciever(zmq::message_t &reciever);

    service_ptr_t lookup_service(const std::string& name);


private:
    context_ptr_t m_ctx;

    // socket for senders and recievers
    socket_ptr_t m_socket;
    service_map_t m_services;



    broker_t(const broker_t&);
    void operator=(const broker_t&);
};

}

#endif // BROKER_HPP
