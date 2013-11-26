#include "endpoint.hpp"

#include <map>
#include <utility>

namespace zbroker {

std::string endpoint_t::connection_string() const {
    const size_t protocol_suffix_size = 10; // a bit higher than it actually is

    std::string connection;
    connection.reserve(protocol_suffix_size + m_address.size());

    switch(m_transport) {
    case TT_INPROC:
        connection.append("inproc");
        break;
    case TT_TCP:
        connection.append("tcp");
        break;
    case TT_IPC:
    default:
        connection.append("ipc");
        break;
    }

    connection.append("://");
    connection.append(m_address);
    return connection;
}

void endpoint_t::swap(endpoint_t &other) {
    std::swap(m_address, other.m_address);
    std::swap(m_transport, other.m_transport);
}

endpoint_t::endpoint_t(const endpoint_t &rhs)
    : m_transport(rhs.m_transport),
      m_address(rhs.m_address)
{
}

endpoint_t::endpoint_t(endpoint_t&& rhs)
    : m_transport(rhs.m_transport),
      m_address(rhs.m_address)
{
}

endpoint_t& endpoint_t::operator =(endpoint_t rhs) {
    if(this != &rhs) {
        swap(rhs);
    }
    return *this;
}

std::ostream& operator << (std::ostream& os, const endpoint_t& ep) {
    os << ep.connection_string();
    return os;
}

}
