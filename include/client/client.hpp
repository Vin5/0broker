#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <string>

#include "data_container.hpp"

#include <boost/shared_ptr.hpp>

class sender_iface_t {
public:
    virtual ~sender_iface_t() { }
    void send(const std::string&);
    void send(const std::vector<std::string>&);
private:
    virtual void send(data_container_t&) = 0;
};

typedef boost::shared_ptr<sender_iface_t> sender_ptr_t;

class receiver_iface_t {
public:
    virtual ~receiver_iface_t() { }
    void recv(std::vector<std::string>&);
private:
    virtual void recv(data_container_t&) = 0;
};

typedef boost::shared_ptr<receiver_iface_t> receiver_ptr_t;

#endif // CLIENT_HPP
