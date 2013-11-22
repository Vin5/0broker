#ifndef BROKER_HPP
#define BROKER_HPP

#include <set>
#include <map>
#include <string>
#include <vector>
#include <deque>

#include "context.hpp"
#include "socket.hpp"
#include <ctime>

namespace zbroker {



class broker_t {
    typedef std::set<std::string> addresses_t;
    typedef std::map<std::string, addresses_t> consumers_t;

    typedef boost::shared_ptr<zmq::message_t> msg_ptr_t;
    typedef std::vector<msg_ptr_t> msg_pack_t;
    typedef std::map<std::string, std::deque<msg_pack_t> > pending_messages_t;

    struct recipient_t {
        msg_ptr_t identity;
        time_t expiry;
    };

    struct service_t {
        std::string name;
        std::deque<msg_pack_t> messages;
        std::deque<recipient_t> waiting;
    };

    typedef boost::shared_ptr<service_t> service_ptr_t;

public:
    broker_t(const context_ptr_t& ctx);

    void run();

private:
    bool handle_frontend(socket_t& frontend);
    void append_pending_messages(const std::string& destination, msg_pack_t&& messages);

    bool handle_backend(socket_t& backend);
    void append_consumer(const std::string& destination, std::string&& consumer_address);

    bool distribute_messages(socket_t& backend);
    bool send_msg_pack(socket_t& backend, const std::string& address, const msg_pack_t& messages);

private:
    context_ptr_t m_ctx;

    consumers_t m_consumers;
    pending_messages_t m_pending_messages;

    // socket for senders and recievers
    socket_ptr_t m_socket;
    std::map<std::string, service_ptr_t> m_services;



    broker_t(const broker_t&);
    void operator=(const broker_t&);
};

}

#endif // BROKER_HPP
