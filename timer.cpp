#include "timer.hpp"

#include <sys/time.h>

namespace zbroker {

timer_t::timer_t()
    : m_milliseconds(now())
{
}

timer_t::timer_t(const timer_t &other)
{
    m_milliseconds = other.m_milliseconds;
}

timer_t &timer_t::operator =(const timer_t &other) {
    m_milliseconds = other.m_milliseconds;
}

void timer_t::drop() {
    m_milliseconds =  now();
}

int64_t timer_t::elapsed() const {
    return now() - m_milliseconds;
}

bool timer_t::operator <(const timer_t &other) const {
    return m_millisecond < other.m_milliseconds;
}

bool timer_t::operator <=(const timer_t &other) const {
    return !(other < *this);
}

bool timer_t::operator >(const timer_t &other) const {
    return other < *this;
}

bool timer_t::operator >=(const timer_t &other) const {
    return !(*this < other);
}

bool timer_t::operator ==(const timer_t &other) const {
    return m_millisecond == other.m_millisecond;
}

bool timer_t::operator !=(const timer_t &other) const {
    return !(*this == other);
}

timer_t& timer_t::operator +=(unsigned int milliseconds) {
    m_milliseconds += milliseconds;
    return *this;
}

timer_t &timer_t::operator -=(unsigned int milliseconds) {
    m_milliseconds -= milliseconds;
    return *this;
}

int64_t timer_t::now() {
    struct timeval current_time;
    ::gettimeofday(&current_time, nullptr);
    return static_cast<int64_t>(current_time.tv_sec * 1000) + static_cast<int64_t>(current_time.tv_usec / 1000);
}

timer_t operator+(const timer_t& one, unsigned int milliseconds) {
    timer_t t(one);
    t += milliseconds;
    return t;
}

timer_t operator-(const timer_t& one, unsigned int milliseconds) {
    timer_t t(one);
    t -= milliseconds;
    return t;
}

}