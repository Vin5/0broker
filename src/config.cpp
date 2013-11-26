#include "config.hpp"




namespace zbroker {



config_t::config_t()
    : m_address(default_configuration::address),
      m_logger_type(default_configuration::logger_type),
      m_is_daemon(default_configuration::is_daemon),
      m_heartbeat_interval(default_configuration::heartbeat_interval)
{
}

const endpoint_t& config_t::address() const {
    return m_address;
}

logger_type_e config_t::logger_type() const {
    return m_logger_type;
}

bool config_t::is_daemon() const {
    return m_is_daemon;
}

size_t config_t::heartbeat_interval() const {
    return m_heartbeat_interval;
}

}
