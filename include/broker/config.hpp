#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "logger.hpp"
#include "endpoint.hpp"

namespace zbroker {

class config_t {
public:
    config_t();


    // address that server is bind to
    const endpoint_t& address() const;

    logger_type_e logger_type() const;

    bool is_daemon() const;

    size_t heartbeat_interval() const;
    size_t liveness() const;

private:
    endpoint_t m_address;
    logger_type_e m_logger_type;
    bool m_is_daemon;
    size_t m_heartbeat_interval;
    size_t m_liveness;
};

namespace default_configuration {
    static const endpoint_t address(TT_TCP, "127.0.0.1:5555"); // broker binds to this endpoint
    static const logger_type_e logger_type = LT_STDOUT;
    static const bool is_daemon = false; // whether or no I am a daemon
    static const size_t heartbeat_interval = 2500; // msecs
    static const size_t heartbeat_liveness = 3; // broker announces it's alive every (heartbeat_liveness * heartbeat_interval) msecs
}

} // zbroker

#endif // CONFIG_HPP
