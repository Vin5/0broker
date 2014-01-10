#include "data_container.hpp"

data_container_t::data_container_t() {
}

data_container_t::data_container_t(const std::string &data) {
    set_data(data);
}

data_container_t::data_container_t(const data_list_t &data) {
    set_data(data);
}

void data_container_t::set_data(const std::string &data) {
    data_list_t data_container;
    data_container.push_back(data);
    set_data(data_container);
}

void data_container_t::set_data(const data_list_t &data) {
    data_list_t::const_iterator i, end = data.end();
    for(i = data.begin(); i != end; ++i) {
        zmq::message_t* msg = new zmq::message_t(i->size());
        memcpy(msg->data(), (void*)i->c_str(), i->size());
        m_messages.push_back(msg);
    }
}

void data_container_t::append_data(const std::string &msg) {
    std::auto_ptr<zmq::message_t> msg_ptr(new zmq::message_t(msg.size()));
    memcpy(msg_ptr->data(), (void*)msg.c_str(), msg.size());
    append_data(msg_ptr);
}

void data_container_t::append_data(zmq::message_t *msg) {
    m_messages.push_back(msg);
}

void data_container_t::append_data(std::auto_ptr<zmq::message_t> msg) {
    append_data(msg.release());
}

void data_container_t::get_data(data_list_t &data_container) const {
    data_container.resize(m_messages.size());
    data_list_t::iterator dest = data_container.begin();
    message_list_t::const_iterator i, end = m_messages.end();
    for(i = m_messages.begin(); i != end; ++i) {
        const zmq::message_t& msg = *i;
        dest->assign((const char*)msg.data(), msg.size());
        ++dest;
    }
}

bool data_container_t::empty() const {
    return m_messages.empty();
}

data_container_t::iterator data_container_t::begin() {
    return m_messages.begin();
}

data_container_t::iterator data_container_t::end() {
    return m_messages.end();
}
