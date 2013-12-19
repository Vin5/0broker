#ifndef LOGGER_HPP
#define LOGGER_HPP


#include <sstream>

namespace zbroker {

enum logger_type_e {
    LT_STDOUT,
    LT_FILE,
    LT_SYSLOG,
    LT_EMPTY // disable logging
};

enum log_level_e {
    LL_INFO = 0,
    LL_WARNING,
    LL_ERROR,
    LL_DEBUG
};

struct log_message_t;

class logger_t {
public:
    virtual ~logger_t() { }
    virtual void log(const log_message_t& msg) = 0;
};

struct log_message_t {
    log_message_t(log_level_e level, logger_t& logger);

    log_message_t(const log_message_t& msg);
    log_message_t& operator=(const log_message_t& msg);


    ~log_message_t();

    template<class T>
    log_message_t& operator<<(T msg) {
        m_stream << msg;
        return *this;
    }

    inline std::string value() const {
        return m_stream.str();
    }

    inline log_level_e level() const {
        return m_level;
    }


private:

    const char* prefix();

    log_level_e m_level;
    logger_t& m_logger;
    std::stringstream m_stream;
};

// log to stdout
class console_logger_t : public logger_t {
    void log(const log_message_t& msg);
};

// dummy
class file_logger_t : public logger_t {
    void log(const log_message_t& msg) {}
};

// dummy
class syslog_logger_t : public logger_t {
    void log(const log_message_t& msg) {}
};

// no logging
class empty_logger_t : public logger_t {
    void log(const log_message_t&) {
        // intentionally left empty
    }
};

} // zbroker

#endif // LOGGER_HPP
