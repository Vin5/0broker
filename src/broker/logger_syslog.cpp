#include <logger.hpp>

#include <syslog.h>

namespace zbroker {

syslog_logger_t::syslog_logger_t(const std::string &identity) {
    ::openlog(identity.c_str(), LOG_PID, LOG_USER);
}

syslog_logger_t::~syslog_logger_t() {
    ::closelog();
}

void syslog_logger_t::log(const log_message_t &msg) {

    switch(msg.level()) {
    case LL_DEBUG:
        ::syslog(LOG_DEBUG, "%s %s", msg.timestamp().c_str(), msg.value().c_str());
        break;
    case LL_INFO:
        ::syslog(LOG_INFO, "%s %s", msg.timestamp().c_str(), msg.value().c_str());
        break;
    case LL_WARNING:
        ::syslog(LOG_WARNING, "%s %s", msg.timestamp().c_str(), msg.value().c_str());
        break;
    case LL_ERROR:
        ::syslog(LOG_ERR, "%s %s", msg.timestamp().c_str(), msg.value().c_str());
        break;
    }
}

} // zbroker
