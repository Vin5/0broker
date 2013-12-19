#include "timer.hpp"

#include <sys/time.h>

namespace zbroker {

// returns current time in milliseconds since epoch
static int64_t current_time() {
    struct timeval current_time;
    ::gettimeofday(&current_time, nullptr);
    return static_cast<int64_t>(current_time.tv_sec * 1000) + static_cast<int64_t>(current_time.tv_usec / 1000);
}

time_point_t::time_point_t()
    : m_milliseconds(current_time())
{
}

time_point_t::time_point_t(const time_point_t &other) {
    m_milliseconds = other.m_milliseconds;
}

time_point_t &time_point_t::operator =(const time_point_t &other) {
    m_milliseconds = other.m_milliseconds;
    return *this;
}

bool time_point_t::operator <(const time_point_t &other) const {
    return m_milliseconds < other.m_milliseconds;
}

bool time_point_t::operator <=(const time_point_t &other) const {
    return !(other < *this);
}

bool time_point_t::operator >(const time_point_t &other) const {
    return other < *this;
}

bool time_point_t::operator >=(const time_point_t &other) const {
    return !(*this < other);
}

bool time_point_t::operator ==(const time_point_t &other) const {
    return m_milliseconds == other.m_milliseconds;
}

bool time_point_t::operator !=(const time_point_t &other) const {
    return !(*this == other);
}

time_point_t& time_point_t::operator +=(unsigned int milliseconds) {
    m_milliseconds += milliseconds;
    return *this;
}

time_point_t &time_point_t::operator -=(unsigned int milliseconds) {
    m_milliseconds -= milliseconds;
    return *this;
}

time_point_t operator+(const time_point_t& one, unsigned int milliseconds) {
    time_point_t t(one);
    t += milliseconds;
    return t;
}

time_point_t operator-(const time_point_t& one, unsigned int milliseconds) {
    time_point_t t(one);
    t -= milliseconds;
    return t;
}

int64_t operator-(const time_point_t& one, const time_point_t& two) {
    return one.m_milliseconds - two.m_milliseconds;
}

int64_t timer_t::elapsed() const {
    return time_point_t() - m_start_time;
}

void timer_t::drop() {
    m_start_time = time_point_t();
}

}
