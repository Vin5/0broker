#ifndef FORWARDS_HPP
#define FORWARDS_HPP

#include <memory>

namespace zmq {
    class context_t;
    class socket_t;
    class message_t;
}

namespace zbroker {
    class context_t;
    class config_t;
    class socket_t;
    class logger_t;
    class endpoint_t;
    class message_pack_t;
    class recipient_t;
    class service_t;

    typedef std::shared_ptr<context_t> context_ptr_t;
    typedef std::shared_ptr<socket_t> socket_ptr_t;
    typedef std::unique_ptr<zmq::message_t> message_part_t;
    typedef std::shared_ptr<recipient_t> recipient_ptr_t;
    typedef std::shared_ptr<service_t> service_ptr_t;
}

#endif // FORWARDS_HPP
