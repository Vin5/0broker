#include "logger.hpp"

#include <string>
#include <iostream>

namespace zbroker {

log_message_t::log_message_t(log_level_e level, logger_t &logger)
    : m_level(level),
      m_logger(logger)
{
    m_stream << "[" << prefix() << "] ";
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

const char *log_message_t::prefix() {
    static const char* str_level[] = {"INFO", "WARNING", "ERROR", "DEBUG"};
    return str_level[m_level];
}

void console_logger_t::log(const log_message_t &msg) {
    std::cout << msg.value() << std::endl;
}


} // zbroker
