#ifndef BROKER_HPP
#define BROKER_HPP

#include <map>
#include <string>

#include <list>

#include "forwards.hpp"
#include "message.hpp"

#include <ctime>

namespace zbroker {

class broker_t {


    struct recipient_t {
        message_part_t identity;
        time_t expiry;
    };

    typedef boost::shared_ptr<recipient_t> recipient_ptr_t;
    typedef boost::weak_ptr<recipient_t> recipient_weak_t;
    typedef std::map<std::string, recipient_ptr_t> recipient_map_t;

    struct service_t {
        explicit service_t(const std::string& name)
            : name (name)
        {
        }

        void dispatch(const socket_ptr_t& backend, message_pack_t&& message);

        std::string name;
        std::list<message_pack_t> messages;
        std::list<recipient_t> waiting;
    };

    typedef boost::shared_ptr<service_t> service_ptr_t;
    typedef std::map<std::string, service_ptr_t> service_map_t;


public:
    broker_t(const context_ptr_t& ctx);

    void run();

    void interrupt();

private:
    bool handle_request();
    bool handle_sender(zmq::message_t &sender);
    bool handle_receiver(zmq::message_t &reciever);

    service_ptr_t lookup_service(const std::string& name);


private:
    context_ptr_t m_ctx;

    // socket for senders and recievers
    socket_ptr_t m_socket;
    service_map_t m_services;
    recipient_map_t m_recipients;

    // indicates the broker was interrupted by a signal
    volatile bool m_interrupted;

    broker_t(const broker_t&);
    void operator=(const broker_t&);
};

}

#endif // BROKER_HPP
