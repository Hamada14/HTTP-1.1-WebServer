#include "../headers/Server.h"
#include "../headers/ServerWorker.h"
#include <stdio.h>
#include <cstdlib>
#include <thread>
#include <unistd.h>

const int Server::DEFAULT_PORT_ = 8080;
const int Server::THREADS_COUNT_ = 6;

Server::Server(): port_(DEFAULT_PORT_), pool(THREADS_COUNT_) {
    configure_address();
}

Server::Server(int port): port_(port), pool(THREADS_COUNT_) {
    configure_address();
}

void Server::configure_address() {
    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(this->port_);
}

int Server::create_socket_descriptor() {
    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    int socket_option = 1;
    if (socket_descriptor == 0) {
        perror("Couldn't create socket");
        exit(EXIT_FAILURE);
    }
    //struct timeval timeout;
    //timeout.tv_sec = 3;
    //timeout.tv_usec = 0;

    setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socket_option, sizeof(socket_option));
    //setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    //setsockopt(socket_descriptor, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

    if (bind(socket_descriptor, (struct sockaddr *)&address_, sizeof(address_))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return socket_descriptor;
}

#include <iostream>
void Server::run() {
    int socket_descriptor = create_socket_descriptor();
    int address_len = sizeof(address_);

    while(true) {
        if (listen(socket_descriptor, 3) < 0)
        {
            perror("Unable to listen using the socket");
            exit(EXIT_FAILURE);
        }
        int new_socket_listener = accept(socket_descriptor, (struct sockaddr *)&address_, (socklen_t*)&address_len);
        if (new_socket_listener < 0) {
            perror("Unable to accept the connection"); 
            exit(EXIT_FAILURE);
        }
        pool.enqueue( [new_socket_listener] {
                printf("New Request Received\n");
                fflush(stdout);
                ServerWorker* worker = new ServerWorker(new_socket_listener);
                worker->process();
                close(new_socket_listener);
        });
    }
}
