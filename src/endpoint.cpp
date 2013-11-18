#include "endpoint.hpp"

#include <map>

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

}
