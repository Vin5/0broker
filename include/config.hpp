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

private:
    endpoint_t m_address;
    logger_type_e m_logger_type;
    bool m_is_daemon;
};

namespace default_configuration {
    static const endpoint_t address(TT_IPC, "127.0.0.1:12345");
    static const logger_type_e logger_type = LT_STDOUT;
    static const bool is_daemon = false;
}

} // zbroker

#endif // CONFIG_HPP
