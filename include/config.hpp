#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "logger.hpp"

namespace zbroker {

class config_t {
public:
    config_t();

    logger_type_e logger_type() const;

    bool is_daemon() const;

private:
     logger_type_e m_logger_type;
     bool m_is_daemon;
};

namespace default_configuration {
    static const logger_type_e logger_type = LT_STDOUT;
    static const bool is_daemon = false;
}

} // zbroker

#endif // CONFIG_HPP
