#ifndef CODES_HPP
#define CODES_HPP

namespace zbroker {

//
//
// every message coming to broker must be
// structured in the following way:
//      _______________________
//     |   sender identity     |
//      -----------------------
//     |      empty part       |
//      -----------------------
//     |         header        |
//      -----------------------
//     |        command        |
//      -----------------------
//     |      destination      |   -- optional
//      -----------------------
//     |    payload (part 1)   |   -- optional
//      -----------------------
//     |          ...          |   -- optional
//      -----------------------
//     |    payload (part n)   |   -- optional
//      -----------------------
//
//  sender identity - uniquely identifies the message sender
//  empty part - follows ZeroMQ message format requirements (REQ sockets)
//  header - means a type of sender
//  command - RPC codes for message dispatching
//  destination - where the payload have to go to
//  payload - useful data of the message


// following control codes are needed for message dispatching
namespace codes {

    namespace header {
        static const char* const sender     = "001";
        static const char* const receiver   = "002";
        static const char* const publisher  = "003";
        static const char* const subscriber = "004";
    } // header

    namespace control {

        namespace broker {
            static const char* const data =          "001";
            static const char* const heartbeat =     "002";
        } // broker

        namespace sender {
            static const char* const put =          "001";
        } // sender

        namespace receiver {
            static const char* const ready =        "001";
            static const char* const reply =        "002";
            static const char* const heartbeat =    "003";
            static const char* const disconnect =   "004";
        } //receiver

    } // control

} // codes

} // zbroker

#endif // CODES_HPP
