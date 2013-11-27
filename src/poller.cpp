#include "poller.hpp"

namespace zbroker {

void poller_t::add(socket_t &sock) {
    zmq::pollitem_t item = {sock, 0, ZMQ_POLLIN, 0};
    m_poll_items.push_back(item);
}

/*
void poller_t::set_sockets(const socket_set_t &sock_set) {
    if(sock_set.empty()) {
        return;
    }

    clear();
    m_poll_items.resize(sock_set.size());

    socket_set_t::iterator next_socket = sock_set.begin();
    for(size_t i = 0; i < m_poll_items.size(); i++) {
        socket_ptr_t sock = *next_socket++;
        items[i].socket = *sock;
        items[i].fd = 0;
        items[i].events = ZMQ_POLLIN;
        items[i].revents = 0;
    }
}*/

static bool is_data_ready(int type, zmq::pollitem_t& item) {
    if ((type & item.revents) != type) {
        return false;
    }
    return true;
}


bool poller_t::poll_in(int timeout) {
    if(m_poll_items.empty()) {
        return false;
    }

    int rc = zmq_poll(&m_poll_items[0], m_poll_items.size(), timeout); // msec

    if (rc < 0) {
        return false;
    }

    return true;
}

bool poller_t::check(size_t index, int flags) {
    if(index >= items_count()) {
        return false;
    }
    return is_data_ready(flags, m_poll_items[index]);
}

size_t poller_t::items_count() const {
    return m_poll_items.size();
}

void poller_t::clear() {
    pollitems_t temp;
    m_poll_items.swap(temp);
}

} // zbroker
