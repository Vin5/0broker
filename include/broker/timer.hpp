#ifndef TIMER_HPP
#define TIMER_HPP

#include <cstdint>

namespace zbroker {

// simple time point based on milliseconds elapsed since epoch begining
class time_point_t
{
    friend int64_t operator-(const time_point_t& one, const time_point_t& two);
public:
    // initialize time_point_t object with current time
    time_point_t();

    time_point_t(const time_point_t& other);
    time_point_t& operator=(const time_point_t& other);

    bool operator <(const time_point_t& other) const;
    bool operator <=(const time_point_t& other) const;
    bool operator >(const time_point_t& other) const;
    bool operator >=(const time_point_t& other) const;
    bool operator ==(const time_point_t& other) const;
    bool operator !=(const time_point_t& other) const;

    time_point_t& operator +=(unsigned int milliseconds);
    time_point_t& operator -=(unsigned int milliseconds);

private:
    int64_t m_milliseconds; // since epoch
};

time_point_t operator+(const time_point_t& one, unsigned int milliseconds);
time_point_t operator-(const time_point_t& one, unsigned int milliseconds);
int64_t operator-(const time_point_t& one, const time_point_t& two);

class timer_t {
public:
    timer_t();

    // drops current timer and begins time tracking again
    void drop();

    // returns milliseconds elapsed since object creation
    int64_t elapsed() const;

private:
    time_point_t m_start_time;

    // we needn't to copy timer objects
    timer_t(const timer_t&);
    timer_t operator=(const timer_t&);
};

} // zbroker

#endif // TIMER_HPP
