#ifndef LOGGER_HPP
#define LOGGER_HPP

namespace zbroker {

enum logger_type_e {
    LT_STDOUT,
    LT_FILE,
    LT_SYSLOG
};

enum log_level_e {
    LL_INFO,
    LL_WARNING,
    LL_ERROR,
    LL_DEBUG
};

} // zbroker

#endif // LOGGER_HPP
