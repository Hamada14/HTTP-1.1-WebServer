#include "../headers/ServerWorker.h"

ServerWorker::ServerWorker(int socket_identifier) {
    this->socket = new Socket(socket_identifier);
}

ServerWorker::~ServerWorker() {
    delete this->socket;
}

#include <iostream>

void ServerWorker::process() {
    while(true) {
        Request* request = socket->readNextRequest();
        request->process(this->socket);
        std::cout << "Another Request" << std::endl;
    }
}