#ifndef RECIPIENT_HPP
#define RECIPIENT_HPP


#include <string>

#include "forwards.hpp"
#include "timer.hpp"

namespace zbroker {

// broker client who wants to get a message
class recipient_t {
public:
    explicit recipient_t(const std::string& id)
        : m_identity(id),
          m_disconnected(false)
    {
    }

    const std::string& identity() const;
    bool expired() const;
    bool disconnected() const;

    void disconnect(); // set the recipient state to disconnected

    void update_expiration(const time_point_t& next_expiration);

private:
    std::string m_identity;     // recipient "address"
    time_point_t m_expiry;      // after the time recipient is considered dead
    bool m_disconnected;        // recipient has been served or disconnected by request

private:
    recipient_t(recipient_t&);
    void operator=(recipient_t&);
};
}

#endif // RECIPIENT_HPP
