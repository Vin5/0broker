#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <stdexcept>
#include <string>
#include <cstdio>

namespace zbroker {

struct runtime_error_t : public std::exception {

    template<typename... Args>
    runtime_error_t(const char* format, const Args&... args) {
        char details[2048];
        m_details.reserve(sizeof(details));
        if(snprintf(details, sizeof(details), format, args...) < 0) {
            sprintf(details, "%s", "Couldn't format error message");
        }
        m_details.assign(details, sizeof(details));
    }

    const char* what() const throw() {
        return m_details.c_str();
    }
private:
    std::string m_details;
};

}

#endif // ERRORS_HPP
