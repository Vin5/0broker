#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "forwards.hpp"
#include "logger.hpp"

namespace zbroker {

class context_t {
public:
    typedef boost::shared_ptr<zmq::context_t> zmq_context_ptr_t;
    typedef boost::shared_ptr<config_t> config_ptr_t;
    typedef boost::shared_ptr<logger_t> logger_ptr_t;

public:
    context_t();

    zmq_context_ptr_t zmq_context() const;
    config_ptr_t config() const;
    log_message_t log(log_level_e level) const;

private:
    void setup_logger();

private:
    zmq_context_ptr_t m_context;
    config_ptr_t m_config;
    logger_ptr_t m_logger;

    context_t(const context_t&);
    void operator=(const context_t&);
};

typedef boost::shared_ptr<context_t> context_ptr_t;

} // zbroker

#endif // CONTEXT_HPP
