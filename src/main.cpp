#include <iostream>

#include "errors.hpp"
#include "broker.hpp"
#include "context.hpp"
#include "logger.hpp"

#include <stdlib.h>
#include <csignal>
#include <stdexcept>

struct signal_handler_t {
    signal_handler_t(zbroker::broker_t& broker) {
        signal_handler_t::broker = &broker;
    }

    void setup() {
        struct sigaction action;
        memset(&action, 0, sizeof(action));
        action.sa_handler = &signal_handler_t::handler;

        if(0 != ::sigaction(SIGQUIT, &action, nullptr))
            throw zbroker::runtime_error_t("SIGQUIT signal handler setup is failed");
        if(0 != ::sigaction(SIGTERM, &action, nullptr))
            throw zbroker::runtime_error_t("SIGTERM signal handler setup is failed");
        if(0 != ::sigaction(SIGINT, &action, nullptr))
            throw zbroker::runtime_error_t("SIGTERM signal handler setup is failed");
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
    try {
        zbroker::broker_t broker(ctx);

        signal_handler_t handler(broker);
        handler.setup();

        broker.run();
    }
    catch(const std::exception& e) {
        ctx->log(zbroker::LL_ERROR) << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
