#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <boost/shared_ptr.hpp>
#include <zmq.hpp>
#include <type_traits>

namespace zbroker {

typedef boost::shared_ptr<zmq::message_t> message_part_t;

template<class T>
struct message_pack_traits {
    // appropriate specialization should be defined
    static void pack(zmq::message_t& dst, const T& src) = delete;
    static void unpack(T& dst, const zmq::message_t& src) = delete;
};

class message_pack_t {
public:

    message_pack_t() {

    }

    template<class T>
    void pop(T& dst) {
        message_part_t part = m_parts.back();
        message_pack_traits<T>::unpack(dst, *part);
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
        message_pack_traits<T>::pack(*part, data);
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

    message_pack_t(const message_pack_t&);
    void operator=(const message_pack_t&);
};


template<>
struct message_pack_traits<std::string> {
    static void pack(zmq::message_t& dst, const std::string& src) {
        zmq::message_t msg(src.size());
        memcpy(msg.data(), src.data(), src.size());
        dst.move(&msg);
    }

    static void unpack(std::string& dst, const zmq::message_t& src) {
        dst.assign(static_cast<const char*>(src.data()), src.size());
    }
};

template<>
struct message_pack_traits<const char*> {
    static void pack(zmq::message_t& dst, const char* src) {
        message_pack_traits<std::string>::pack(dst, src);
    }

    // refuse unpacking to char array in order to prevent an overflow
    static void unpack(char* dst, const zmq::message_t& src) = delete;
};

// implementation is the same as 'const char*'
template<>
struct message_pack_traits<char*> : message_pack_traits<const char*> {

};


template<size_t N>
struct message_pack_traits<char[N]> {
    static void pack(zmq::message_t& dst, const char src[N]) {
        zmq::message_t msg(N);
        memcpy(msg.data(), src, N);
        dst.move(&msg);
    }

    static void unpack(char dst[N], const zmq::message_t& src) {
        size_t min_size = src.size() > N ? N : src.size();
        memcpy(dst, static_cast<const char*>(src.data()), min_size); // loss of data is possible
    }
};

}

#endif // MESSAGE_HPP
