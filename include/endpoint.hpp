#ifndef ENDPOINT_HPP
#define ENDPOINT_HPP

#include <string>

namespace zbroker {

enum transport_type_e {
    TT_INPROC,
    TT_IPC,
    TT_TCP
};

class endpoint_t{
public:
    endpoint_t(transport_type_e transport, const std::string& address);

    std::string connection_string() const;

private:
    transport_type_e m_transport;
    std::string m_address;
};

} // zbroker

#endif // ENDPOINT_HPP
