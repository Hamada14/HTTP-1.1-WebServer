#include <iostream>

#include "headers/Server.h"
#include "headers/Config.h"

int main(int argc, const char** argv) {
    Config* program_config = Config::getInstance();
    program_config->init(argc, argv);
    Server server;
    server.run();
    return 0;
}