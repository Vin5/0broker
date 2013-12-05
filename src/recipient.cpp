#include "recipient.hpp"

namespace zbroker {

message_part_t recipient_t::identity() const {
    return m_identity;
}

bool recipient_t::expired() const {
    return time_point_t() > m_expiry;
}

bool recipient_t::disconnected() const {
    return m_disconnected;
}

void recipient_t::disconnect() {
    m_disconnected = true;
}

void recipient_t::update_expiration(const time_point_t& next_expiration) {
    m_expiry = next_expiration;
}

} // zbroker
