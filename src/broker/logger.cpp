#include "logger.hpp"

#include <string>
#include <iostream>
#include <syslog.h>

namespace zbroker {

log_message_t::log_message_t(log_level_e lvl, logger_t &logger)
    : m_level(lvl),
      m_logger(logger)
{
    m_stream << "[" << level_to_str(m_level) << "] ";
}

log_message_t::log_message_t(const log_message_t& msg)
    : m_level(msg.m_level),
      m_logger(msg.m_logger)
{
    m_stream << msg.m_stream.str();
}

log_message_t& log_message_t::operator=(const log_message_t& msg) {
    m_level = msg.m_level;
    m_logger = msg.m_logger;
    m_stream << msg.m_stream.str();
    return *this;
}

log_message_t::~log_message_t() {
    try {
        switch(m_level) {
        case LL_DEBUG:
#ifndef _DEBUG
            break; // log debug messages only in debug build mode
#endif
        default:
            m_logger.log(*this);
            break;
        }
    }
    catch(...) { }
}

std::string log_message_t::level_to_str(log_level_e lvl) {
    static const char* str_level[] = {"INFO", "WARNING", "ERROR", "DEBUG"};
    return str_level[lvl];
}

void console_logger_t::log(const log_message_t &msg) {
    std::cout << msg.value() << std::endl;
}


file_logger_t::file_logger_t(const std::string &filename)
    : m_filename(filename) {

}

syslog_logger_t::syslog_logger_t(const std::string &identity) {
    ::openlog(identity.c_str(), LOG_PID, LOG_USER);
}

syslog_logger_t::~syslog_logger_t() {
    ::closelog();
}

void syslog_logger_t::log(const log_message_t &msg) {

    switch(msg.level()) {
    case LL_DEBUG:
        ::syslog(LOG_DEBUG, "%s", msg.value().c_str());
        break;
    case LL_INFO:
        ::syslog(LOG_INFO, "%s", msg.value().c_str());
        break;
    case LL_WARNING:
        ::syslog(LOG_WARNING, "%s", msg.value().c_str());
        break;
    case LL_ERROR:
        ::syslog(LOG_ERR, "%s", msg.value().c_str());
        break;
    }
}

} // zbroker
