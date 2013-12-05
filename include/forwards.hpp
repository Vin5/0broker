#ifndef FORWARDS_HPP
#define FORWARDS_HPP

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

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

    typedef boost::shared_ptr<context_t> context_ptr_t;
    typedef boost::shared_ptr<socket_t> socket_ptr_t;
    typedef boost::shared_ptr<zmq::message_t> message_part_t;
}

#endif // FORWARDS_HPP
