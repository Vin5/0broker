#ifndef ENDPOINT_HPP
#define ENDPOINT_HPP

#include <string>

namespace zbroker {

enum transport_type_e {
    TT_INPROC,
    TT_IPC,
    TT_TCP
};

class endpoint_t {
public:
    endpoint_t(transport_type_e transport, const std::string& address)
        : m_transport(transport),
          m_address(address)
    {
    }


    endpoint_t(const endpoint_t& rhs);
    endpoint_t(endpoint_t&& rhs);
    endpoint_t& operator=(endpoint_t rhs);

    void swap(endpoint_t& other);

    std::string connection_string() const;

private:
    transport_type_e m_transport;
    std::string m_address;
};

} // zbroker

#endif // ENDPOINT_HPP
