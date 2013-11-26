#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <boost/shared_ptr.hpp>

#include "msg_traits.hpp"

namespace zbroker {

namespace message {

    template<class T>
    inline void pack(zmq::message_t& dst, const T& src) {
        message_traits<T>::pack(dst, src);
    }

    template<class T>
    inline void unpack(T& dst, const zmq::message_t& src) {
        message_traits<T>::unpack(dst, src);
    }

    template<class T>
    inline bool equal_to(const zmq::message_t& message, const T& data) {
        return message_traits<T>::equals(message, data);
    }

}

typedef boost::shared_ptr<zmq::message_t> message_part_t;

class message_pack_t {
public:

    message_pack_t() {

    }

    template<class T>
    void pop(T& dst) {
        message_part_t part = m_parts.back();
        message::unpack(dst, *part);
        m_parts.pop_back();
    }

    message_part_t pop() {
        message_part_t part = m_parts.back();
        m_parts.pop_back();
        return part;
    }


    void push(const message_part_t& part) {
        m_parts.push_back(part);
    }

    template<class T>
    void push(const T& data) {
        message_part_t part(new zmq::message_t);
        message::pack(*part, data);
        m_parts.push_back(part);
    }

    size_t size() const {
        return m_parts.size();
    }
    bool empty() const {
        return m_parts.empty();
    }

private:
    std::vector<message_part_t> m_parts;
};




}

#endif // MESSAGE_HPP
