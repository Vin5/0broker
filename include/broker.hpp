#ifndef BROKER_HPP
#define BROKER_HPP

#include <map>
#include <string>

#include <list>

#include "forwards.hpp"
#include "message.hpp"

#include "timer.hpp"

namespace zbroker {

class broker_t {

    typedef std::map<std::string, recipient_ptr_t> recipient_map_t;
    typedef std::map<std::string, service_ptr_t> service_map_t;

public:
    broker_t(const context_ptr_t& ctx);

    void run();

    // signal handler calls this function
    void interrupt();

private:
    bool handle_request();
    void handle_sender(message_pack_t& msg);
    void handle_receiver(message_pack_t& msg);

    service_ptr_t lookup_service(const std::string& name);
    recipient_ptr_t lookup_recipient(const message_part_t& receiver);

    // returns time point the next expiration occurs at
    time_point_t next_expiration();

    void renew_recipients();
    void send_heartbeat(const recipient_ptr_t& recipient);

private:
    context_ptr_t m_ctx;


    socket_ptr_t m_socket; // socket for senders and recievers
    service_map_t m_services; // known services
    recipient_map_t m_recipients; // active recipients
    time_point_t m_heartbeat_time; // time to send a heartbeat to active recipients and remove stale ones

    volatile bool m_interrupted; // broker was interrupted by a signal

private:
    broker_t(const broker_t&);
    void operator=(const broker_t&);
};

}

#endif // BROKER_HPP
