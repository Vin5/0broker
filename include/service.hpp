#ifndef SERVICE_HPP
#define SERVICE_HPP

#include "forwards.hpp"
#include "message.hpp"

#include <string>
#include <list>

namespace zbroker {

class service_t {
public:
    explicit service_t(const std::string& name)
        : name (name)
    {
    }

    void attach_waiter(const recipient_ptr_t& waiter);
    void append_message(message_pack_t&& message);

    void dispatch(const socket_ptr_t& backend);

private:
    void send_message(const socket_ptr_t& backend, const recipient_ptr_t& recipient, message_pack_t &msg);
private:
    std::string name;
    std::list<message_pack_t> messages;
    std::list<recipient_ptr_t> waiting;

private:
    service_t(const service_t&);
    void operator=(const service_t&);
};

} // zbroker

#endif // SERVICE_HPP
