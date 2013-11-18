#ifndef POLLER_HPP
#define POLLER_HPP

#include "socket.hpp"

#include <vector>
#include <zmq.hpp>

namespace zbroker {

class poller_t {
    typedef std::vector<zmq::pollitem_t> pollitems_t;
public:


    void add(socket_t& sock);
    void set_sockets(const socket_set_t& sock_set);

    void clear();

    bool poll_in(int timeout);
    bool check(size_t item_index, int flag);

    size_t items_count() const;

private:
    pollitems_t m_poll_items;

private:
    poller_t(const poller_t&);
    void operator=(const poller_t&);
};

} // zbroker

#endif // POLLER_HPP
