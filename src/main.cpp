#include <iostream>

#include "errors.hpp"
#include "broker.hpp"
#include "context.hpp"

#include <stdlib.h>
#include <csignal>
#include <stdexcept>

struct signal_handler_t {
    signal_handler_t(zbroker::broker_t& broker) {
        signal_handler_t::broker = &broker;
    }

    bool setup() {
        struct sigaction action;
        memset(&action, 0, sizeof(action));
        action.sa_handler = &signal_handler_t::handler;

        if(0 != ::sigaction(SIGQUIT, &action, nullptr))
            return false;
        if(0 != ::sigaction(SIGTERM, &action, nullptr))
            return false;
        if(0 != ::sigaction(SIGINT, &action, nullptr))
            return false;
        return true;
    }

    static void handler(int sig) {
        if(broker) {
            broker->interrupt();
        }
    }
    static zbroker::broker_t* broker;
};

zbroker::broker_t* signal_handler_t::broker = nullptr;

int main(int argc, char* argv[]) {

    zbroker::context_ptr_t ctx(new zbroker::context_t);
    zbroker::broker_t broker(ctx);

    signal_handler_t handler(broker);
    if(!handler.setup()) {
        std::cout << "Signals setting up is failed" << std::endl;
    }

    try {
        broker.run();
    }
    catch(std::exception& e) {
        ctx->log(zbroker::LL_ERROR) << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
