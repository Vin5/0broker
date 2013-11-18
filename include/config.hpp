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
    static logger_type_e logger_type = STDOUT_LOGGER;
    static bool is_daemon = false;
}

}

#endif // CONFIG_HPP
