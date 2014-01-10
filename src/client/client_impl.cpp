#include "client_impl.hpp"
#include "errors.hpp"
#include "codes.hpp"
#include "message.hpp"
#include "connection.hpp"

#include <auto_ptr.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#define  BOOST_CHRONO_HEADER_ONLY
#include <boost/chrono/chrono_io.hpp>

sender_impl_t::sender_impl_t(const connection_ptr_t& connection, const std::string &service)
    : sender_t(connection),
      m_service(service)
{
    m_socket = create_socket(ZMQ_DEALER, generate_uuid());
    m_socket->connect(address().c_str());
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


receiver_impl_t::receiver_impl_t(const connection_ptr_t& connection, const std::string &service)
    : receiver_t(connection),
      m_service(service)
{
    m_socket = create_socket(ZMQ_DEALER, generate_uuid());
    m_socket->connect(address().c_str());
}

typedef boost::chrono::system_clock::time_point system_time_t;

// returns current time in milliseconds since epoch
static system_time_t current_time() {
    /*struct timeval current_time;
    ::gettimeofday(&current_time, NULL);
    return static_cast<int64_t>(current_time.tv_sec * 1000) + static_cast<int64_t>(current_time.tv_usec / 1000);*/

    return boost::chrono::system_clock::now();
}

static void send(zmq::socket_t& socket, const std::string& data, int options = 0) {
    zmq::message_t msg(data.size());
    if(!data.empty()) {
        memcpy(msg.data(), (void*)data.c_str(), data.size());
    }
    socket.send(msg, options);
}

static void send_receiver_is_ready(zmq::socket_t& socket, const std::string& destination) {
    send(socket, std::string(), ZMQ_SNDMORE); // empty
    send(socket, zbroker::codes::header::receiver, ZMQ_SNDMORE); // who am I
    send(socket, zbroker::codes::control::receiver::ready, ZMQ_SNDMORE); // what I want
    send(socket, destination); // where I send it
}

void receiver_impl_t::recv(data_container_t & data) {
    send_receiver_is_ready(*m_socket, m_service);

    int liveness = retries_count();
    boost::chrono::milliseconds heartbeat_interval(timeout());
    system_time_t previous_hearbeat = current_time();
    while(liveness) {
        zmq::pollitem_t item[] = {
            {*m_socket, 0, ZMQ_POLLIN, 0}
        };

        zmq::poll(item, 1, timeout());
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
            send_receiver_is_ready(*m_socket, m_service);
            liveness--;
        }
        if(current_time() - previous_hearbeat > heartbeat_interval) {
            send(*m_socket, std::string(), ZMQ_SNDMORE);
            send(*m_socket, zbroker::codes::header::receiver, ZMQ_SNDMORE);
            send(*m_socket, zbroker::codes::control::receiver::heartbeat);;
            previous_hearbeat = current_time();
        }
    }
}

async_receiver_impl_t::~async_receiver_impl_t() {
    try {
        if(m_handler) {
            send(*m_background_manager, "stop");
            zmq::message_t ok;
            m_background_manager->recv(&ok);
        }
    }
    catch(...) {
        std::cout << "~async_receiver_impl_t exception caught" << std::endl;
    }
}

async_receiver_impl_t::async_receiver_impl_t(const connection_ptr_t &connection, const std::string &service)
    : async_receiver_t(connection),
      m_service(service)
{
    m_background_manager = create_socket(ZMQ_PAIR);
    m_background_manager->bind("inproc://management");
}

void async_receiver_impl_t::set_handler(const async_receiver_t::handler_ptr_t &handler) {
    m_handler = handler;
    boost::thread t(boost::bind(&async_receiver_impl_t::background_receiver, this));
    t.detach();
}

void async_receiver_impl_t::background_receiver() {
    // connect to broker
    boost::shared_ptr<zmq::socket_t> socket = create_socket(ZMQ_DEALER, generate_uuid());
    socket->connect(address().c_str());

    // connect to thread manager
    boost::shared_ptr<zmq::socket_t> controller = create_socket(ZMQ_PAIR);
    controller->connect("inproc://management");



    int liveness = retries_count();
    boost::chrono::milliseconds heartbeat_interval(timeout());
    system_time_t previous_hearbeat = current_time();
    bool stopped = false;

    while(liveness && !stopped) {
        // send to broker we are ready to receive data
        send_receiver_is_ready(*socket, m_service);

        zmq::pollitem_t item[] = {
            {*controller, 0, ZMQ_POLLIN, 0},
            {*socket, 0, ZMQ_POLLIN, 0}
        };

        zmq::poll(item, 2, timeout());
        if(item[0].revents & ZMQ_POLLIN) {
            zmq::message_t control_msg;
            controller->recv(&control_msg);
            send(*socket, std::string(), ZMQ_SNDMORE);
            send(*socket, zbroker::codes::header::receiver, ZMQ_SNDMORE);
            send(*socket, zbroker::codes::control::receiver::disconnect);

            send(*controller, "ok");
            stopped = true;
            m_handler->on_disconnect();
        }
        if(item[1].revents & ZMQ_POLLIN) {

            liveness = 3;

            zmq::message_t empty;
            socket->recv(&empty);

            zmq::message_t header;
            socket->recv(&header);

            if(zbroker::message::equal_to(header, zbroker::codes::control::broker::data)) {
                data_container_t data;
                bool more = true;
                while(more) {
                    std::auto_ptr<zmq::message_t> reply(new zmq::message_t) ;
                    socket->recv(reply.get());
                    more = reply->more();
                    data.append_data(reply);
                }
                data_list_t data_transformed;
                data.get_data(data_transformed);
                m_handler->on_recv(data_transformed);
            }
            else if(zbroker::message::equal_to(header, zbroker::codes::control::broker::heartbeat))
                std::cout << "hearbeat" << std::endl;
            else
                std::cout << "Invalid message" << std::endl;
        }
        else if(!stopped){
            send_receiver_is_ready(*socket, m_service);
            liveness--;
        }
        if(current_time() - previous_hearbeat > heartbeat_interval && !stopped) {
            send(*socket, std::string(), ZMQ_SNDMORE);
            send(*socket, zbroker::codes::header::receiver, ZMQ_SNDMORE);
            send(*socket, zbroker::codes::control::receiver::heartbeat);
            previous_hearbeat = current_time();
        }
    }
}
