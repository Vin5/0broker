#ifndef MSG_PACK_TRAITS_HPP
#define MSG_PACK_TRAITS_HPP

#include <zmq.hpp>

namespace zbroker {

template<class T>
struct message_traits;


template<>
struct message_traits<std::string> {
    static void pack(zmq::message_t& dst, const std::string& src) {
        zmq::message_t msg(src.size());
        memcpy(msg.data(), src.data(), src.size());
        dst.move(&msg);
    }

    static void unpack(std::string& dst, const zmq::message_t& src) {
        dst.assign(static_cast<const char*>(src.data()), src.size());
    }

    static bool equals(const zmq::message_t& msg, const std::string& data) {
        if(data.size() != msg.size()) {
            return false;
        }
        return (memcmp(msg.data(), data.data(), msg.size()) == 0);
    }
};

template<>
struct message_traits<const char*> {
    static void pack(zmq::message_t& dst, const char* src) {
        message_traits<std::string>::pack(dst, src);
    }

    // refuse unpacking to char array in order to prevent an overflow
    static void unpack(char* dst, const zmq::message_t& src) = delete;

    static bool equals(const zmq::message_t& msg, const char* data) {
        return (memcmp(msg.data(), data, msg.size()) == 0);
    }
};

// implementation is the same as for 'const char*'
template<>
struct message_traits<char*> : message_traits<const char*> {

};

template<size_t N>
struct message_traits<char[N]> {
    static void pack(zmq::message_t& dst, const char src[N]) {
        zmq::message_t msg(N);
        memcpy(msg.data(), src, N);
        dst.move(&msg);
    }

    static void unpack(char dst[N], const zmq::message_t& src) {
        size_t min_size = src.size() > N ? N : src.size();
        memcpy(dst, static_cast<const char*>(src.data()), min_size); // loss of data is possible, think twice before using
    }

    static bool equals(const zmq::message_t& msg, const char data[N]) {
        const size_t msg_size = msg.size();
        if(N != msg_size) {
            if(N > msg_size && (N - msg_size) == 1) { // we could get string literal and N is length of content + 1 due to '\0'
                return (memcmp(msg.data(), data, N - 1) == 0); // compare the contents only
            }
        }
        return (memcmp(msg.data(), data, msg.size()) == 0);
    }
};

}

#endif // MSG_PACK_TRAITS_HPP
