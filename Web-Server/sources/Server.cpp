#include "../headers/Server.h"
#include "../headers/ServerWorker.h"
#include <condition_variable>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <unistd.h>

const int Server::DEFAULT_PORT_ = 8080;
const int Server::THREADS_COUNT_ = 10;
const int Server::CONNECTION_TIME_OUT_ = 4000; // In millis
const int Server::MAX_ALLOWED_CLIENTS_ = 8;

Server::Server() : port_(DEFAULT_PORT_), pool_(THREADS_COUNT_) {
    configure_address();
}

Server::Server(int port) : port_(port), pool_(THREADS_COUNT_) {
    configure_address();
}

Server::~Server() { stop(); }

void Server::stop() {
    if (is_stopping_)
        return;
    is_stopping_ = true;
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

    setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &socket_option, sizeof(socket_option));

    auto flags = fcntl(socket_descriptor, F_GETFL, 0);
    auto err = fcntl(socket_descriptor, F_SETFL, flags | O_NONBLOCK);
    if (flags < 0 || err < 0) {
        perror("Could set server socket to be non blocking");
        exit(EXIT_FAILURE);
    }

    if (bind(socket_descriptor, (struct sockaddr *)&address_,
             sizeof(address_)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    return socket_descriptor;
}

/**
 * Start Manager Thread
 * Loops over current workers
 * and stops them if they are not updated in a certain amount of time.
 **/
void Server::startManager() {
    pool_.enqueue([this] {
        using namespace std::chrono_literals;
		std::cout << "MANAGER start\n" << std::flush;

        while (!is_stopping_ || !current_workers_.empty()) {
            {
                // CRITICAL SECTION START
                std::unique_lock<std::mutex> lock(workers_mutex_);

                for (auto it = current_workers_.begin();
                     it != current_workers_.end();) {
                    auto &worker = *it;
                    double elapsed_time = clock() - worker->lastUpdated();
                    if (elapsed_time >= CONNECTION_TIME_OUT_) {
                        worker->stop();
                        it = current_workers_.erase(it);
                        std::cout << "MANAGER removed a worker\n" << std::flush;
                    } else {
                        ++it;
                    }
                }
                // CRITICAL SECTION END
            }

            std::this_thread::sleep_for(100ms);
        }
        std::cout << "MANAGER exit\n" << std::flush;
    });
}

void Server::run() {

    int socket_descriptor = create_socket_descriptor();
    int address_len = sizeof(address_);
    if (listen(socket_descriptor, 3) < 0) {
        perror("Unable to listen using the socket");
        exit(EXIT_FAILURE);
    }

    startManager();

    while (true) {
        int new_socket_listener =
            accept(socket_descriptor, (struct sockaddr *)&address_,
                   (socklen_t *)&address_len);

        if (current_workers_.size() == MAX_ALLOWED_CLIENTS_) {
            using namespace std::chrono_literals;

            std::cout << "Reached max clients, sleeping for 1s\n"
                      << std::flush;
            std::this_thread::sleep_for(1s);
            continue;
        }

        if (new_socket_listener < 0 && errno == EWOULDBLOCK) {
            using namespace std::chrono_literals;

            std::cout << "No active connections, sleeping for 1s\n"
                      << std::flush;
            std::this_thread::sleep_for(1s);
            continue;
        }
        if (new_socket_listener < 0) {
            perror("Unable to accept the connection");
            exit(EXIT_FAILURE);
        }

        auto worker = std::make_shared<ServerWorker>(new_socket_listener);

        std::cout << "New Client Connected\n" << std::flush;

        // WRITER
        pool_.enqueue(
            [new_socket_listener, worker] { worker->processRequests(); });

        // READER
        pool_.enqueue(
            [new_socket_listener, worker] { worker->readRequests(); });

        {
            // CRITICAL SECTION START
            std::unique_lock<std::mutex> lock(workers_mutex_);
            current_workers_.emplace_back(worker);
            // CRITICAL SECTION END
        }
    }
}
