#include "context.hpp"
#include "config.hpp"

#include <zmq.hpp>

namespace zbroker {

context_t::context_t() {
    m_context.reset(new zmq::context_t(1));
    m_config.reset(new config_t);
}

context_t::zmq_context_ptr_t context_t::zmq_context() const {
    return m_context;
}

context_t::config_ptr_t context_t::config() const {
    return m_config;
}

}
