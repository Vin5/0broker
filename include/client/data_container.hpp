#ifndef DATA_CONTAINER_HPP
#define DATA_CONTAINER_HPP

#include <string>
#include <vector>
#include <zmq.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <auto_ptr.h>

typedef std::vector<std::string> data_list_t;

class data_container_t {
public:
    typedef boost::ptr_vector<zmq::message_t> message_list_t;
    typedef message_list_t::iterator iterator;

public:

    data_container_t();
    explicit data_container_t(const std::string& data);
    explicit data_container_t(const data_list_t& data);

    void set_data(const std::string& data);
    void set_data(const data_list_t& data);

    void append_data(const std::string& msg);
    void append_data(std::auto_ptr<zmq::message_t> msg);
    void append_data(zmq::message_t* msg);

    void get_data(data_list_t& data) const;

    bool empty() const;

    iterator begin();
    iterator end();

private:
    message_list_t m_messages;
};

#endif // DATA_CONTAINER_HPP
