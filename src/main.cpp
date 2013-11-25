#include <iostream>

#include "broker.hpp"
#include "context.hpp"
#include "message.hpp"

// error prone version
static void broker_prototype_1() {
    zbroker::context_ptr_t ctx(new zbroker::context_t);
    zbroker::broker_t broker(ctx);
    broker.run();
}

int main(int argc, char* argv[]) {
    broker_prototype_1();
    return 0;
}
