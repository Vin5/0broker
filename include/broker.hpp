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

    struct recipient_t {
        recipient_t(const message_part_t& id)
            : m_identity(id),
              m_disconnected(false)
        {
        }

        message_part_t identity() const;
        bool expired() const;
        bool disconnected() const;

        void disconnect();
        void update_expiration(const time_point_t& next_expiration);

    private:
        message_part_t m_identity;
        time_point_t m_expiry;
        bool m_disconnected;
    };

    typedef boost::shared_ptr<recipient_t> recipient_ptr_t;
    typedef std::map<std::string, recipient_ptr_t> recipient_map_t;

    struct service_t {


        explicit service_t(const std::string& name)
            : name (name)
        {
        }

        void attach_waiter(const recipient_ptr_t& waiter);
        void append_message(message_pack_t&& message);

        void dispatch(const socket_ptr_t& backend);

        std::string name;
        std::list<message_pack_t> messages;
        std::list<recipient_ptr_t> waiting;
    };

    typedef boost::shared_ptr<service_t> service_ptr_t;
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

    // socket for senders and recievers
    socket_ptr_t m_socket;
    service_map_t m_services;
    recipient_map_t m_recipients;
    time_point_t m_heartbeat_time;

    // indicates the broker was interrupted by a signal
    volatile bool m_interrupted;

    broker_t(const broker_t&);
    void operator=(const broker_t&);
};

}

#endif // BROKER_HPP
