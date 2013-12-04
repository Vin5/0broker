#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <deque>

#include "forwards.hpp"
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

class message_pack_t {
public:

    template<class T>
    void pop_tail(T& dst) {
        message_part_t part = m_parts.back();
        message::unpack(dst, *part);
        m_parts.pop_back();
    }

    template<class T>
    void pop_head(T& dst) {
        message_part_t part = m_parts.front();
        message::unpack(dst, *part);
        m_parts.pop_front();
    }

    message_part_t pop_tail() {
        message_part_t part = m_parts.back();
        m_parts.pop_back();
        return part;
    }

    message_part_t pop_head() {
        message_part_t part = m_parts.front();
        m_parts.pop_front();
        return part;
    }

    void push_head(const message_part_t& part) {
        m_parts.push_front(part);
    }

    void push_tail(const message_part_t& part) {
        m_parts.push_back(part);
    }

    template<class T>
    void push_tail(const T& data) {
        message_part_t part(new zmq::message_t);
        message::pack(*part, data);
        m_parts.push_back(part);
    }

    message_part_t& operator[](size_t index) {
        return m_parts.at(index);
    }

    size_t size() const {
        return m_parts.size();
    }
    bool empty() const {
        return m_parts.empty();
    }

private:
    std::deque<message_part_t> m_parts;
};

} // zbroker

#endif // MESSAGE_HPP
