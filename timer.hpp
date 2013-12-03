#ifndef TIMER_HPP
#define TIMER_HPP

#include <memory>
#include <cstdint>

namespace zbroker {

// simple timer operating on milliseconds
class timer_t
{
public:
    // initialize timer_t object with current time
    timer_t();

    timer_t(const timer_t& other);
    timer_t& operator=(const timer_t& other);

    // drops current timer and begins time tracking again
    void drop();

    // returns milliseconds elapsed since object creation
    int64_t elapsed() const;

    bool operator <(const timer_t& other) const;
    bool operator <=(const timer_t& other) const;
    bool operator >(const timer_t& other) const;
    bool operator >=(const timer_t& other) const;
    bool operator ==(const timer_t& other) const;
    bool operator !=(const timer_t& other) const;

    timer_t& operator +=(unsigned int milliseconds);
    timer_t& operator -=(unsigned int milliseconds);

private:
    static int64_t now();

    int64_t m_milliseconds; // since epoch
};

timer_t operator+(const timer_t& one, unsigned int milliseconds);
timer_t operator-(const timer_t& one, unsigned int milliseconds);

} // zbroker

#endif // TIMER_HPP
