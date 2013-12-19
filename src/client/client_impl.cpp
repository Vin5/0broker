#include "client_impl.hpp"
#include "errors.hpp"
#include "codes.hpp"
#include "message.hpp"

#include <auto_ptr.h>
#include <sys/time.h>

sender_impl_t::sender_impl_t(const boost::shared_ptr<zmq::socket_t>& sock, const std::string &service)
    : m_service(service),
      m_socket(sock)
{

}

void sender_impl_t::send(data_container_t& data) {
    if(data.empty()) {
        throw zbroker::runtime_error_t("send: %s", "empty data container");
    }

    zmq::socket_t& sock = *m_socket;

    zmq::message_t empty;
    sock.send(empty, ZMQ_SNDMORE);

    std::string header(zbroker::codes::header::sender);
    zmq::message_t header_msg(header.size());
    memcpy(header_msg.data(), (void*)header.data(), header.size());
    sock.send(header_msg, ZMQ_SNDMORE);

    std::string command(zbroker::codes::control::sender::put);
    zmq::message_t command_msg(command.size());
    memcpy(command_msg.data(), (void*)command.data(), command.size());
    sock.send(command_msg, ZMQ_SNDMORE);

    zmq::message_t destination_msg(m_service.size());
    memcpy(destination_msg.data(), (void*)m_service.data(), m_service.size());
    sock.send(destination_msg, ZMQ_SNDMORE);

    data_container_t::iterator message_iterator, end = data.end() - 1;
    for(message_iterator = data.begin(); message_iterator != end; ++message_iterator) {
        sock.send(*message_iterator, ZMQ_SNDMORE);
    }
    sock.send(*message_iterator);
}


receiver_impl_t::receiver_impl_t(const boost::shared_ptr<zmq::socket_t> &sock, const std::string &service)
    : m_service(service),
      m_socket(sock)
{

}

// returns current time in milliseconds since epoch
static int64_t current_time() {
    struct timeval current_time;
    ::gettimeofday(&current_time, NULL);
    return static_cast<int64_t>(current_time.tv_sec * 1000) + static_cast<int64_t>(current_time.tv_usec / 1000);
}

void receiver_impl_t::recv(data_container_t & data) {
    send_readiness();



    int liveness = 3;
    int64_t heartbeat_at = current_time() + 2500;
    while(liveness) {
        zmq::pollitem_t item[] = {
            {*m_socket, 0, ZMQ_POLLIN, 0}
        };

        zmq::poll(item, 1, 2500);
        if(item[0].revents & ZMQ_POLLIN) {

            liveness = 3;

            zmq::message_t empty;
            m_socket->recv(&empty);

            zmq::message_t header;
            m_socket->recv(&header);

            if(zbroker::message::equal_to(header, zbroker::codes::control::broker::data)) {
                bool more = true;
                while(more) {
                    std::auto_ptr<zmq::message_t> reply(new zmq::message_t) ;
                    m_socket->recv(reply.get());
                    more = reply->more();
                    data.append_data(reply);
                }
                break;
            }
            else if(zbroker::message::equal_to(header, zbroker::codes::control::broker::heartbeat))
                std::cout << "hearbeat" << std::endl;
            else
                std::cout << "Invalid message" << std::endl;
        }
        else {
            send_readiness();
            liveness--;
        }
        if(current_time() > heartbeat_at) {
            send_header();
            send_command(zbroker::codes::control::receiver::heartbeat);
            heartbeat_at = current_time() + 2500;
        }
    }
}

void receiver_impl_t::send_header() {
    zmq::message_t empty;
    m_socket->send(empty, ZMQ_SNDMORE);

    zmq::message_t header(3);
    memcpy(header.data(), (void*)zbroker::codes::header::receiver, 3);
    m_socket->send(header, ZMQ_SNDMORE);
}

void receiver_impl_t::send_command(const std::string &command_str, int options) {
    zmq::message_t command(command_str.size());
    memcpy(command.data(), (void*)command_str.data(), command_str.size());
    m_socket->send(command, options);
}

void receiver_impl_t::send_readiness() {
    send_header();
    send_command(zbroker::codes::control::receiver::ready, ZMQ_SNDMORE);

    zmq::message_t destination(m_service.size());
    memcpy(destination.data(), (void*)m_service.data(), m_service.size());
    m_socket->send(destination);
}
