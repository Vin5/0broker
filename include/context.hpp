#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "forwards.hpp"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace zbroker {

class context_t : boost::noncopyable {
public:
    typedef boost::shared_ptr<zmq::context_t> zmq_context_ptr_t;
    typedef boost::shared_ptr<config_t> config_ptr_t;

public:
    context_t();

    zmq_context_ptr_t zmq_context() const;
    config_ptr_t config() const;

private:
    zmq_context_ptr_t m_context;
    config_ptr_t m_config;
};

typedef boost::shared_ptr<context_t> context_ptr_t;

} // zbroker

#endif // CONTEXT_HPP
