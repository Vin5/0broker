#include "context.hpp"

#include <zmq.hpp>

namespace zbroker {

context_t::context_t() {

}

context_t::zmq_context_ptr_t context_t::zmq_context() const {
    return m_context;
}

context_t::config_ptr_t context_t::config() const {
    return m_config;
}

}
