#include "../headers/ServerWorker.h"

ServerWorker::ServerWorker(int socket_identifier): socket(socket_identifier){
}

#include <iostream>

void ServerWorker::process() {
    //while(true) {
        Request* request = socket.readNextRequest();
        request->process(&this->socket);
        std::cout << "Another Request" << std::endl;
    //}
}
