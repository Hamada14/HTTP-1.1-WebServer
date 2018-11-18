#include "../headers/Server.h"
#include "../headers/ServerWorker.h"
#include <stdio.h>
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <iostream>

const int Server::DEFAULT_PORT_ = 8080;
const int Server::THREADS_COUNT_ = 10;
const int Server::READ_TIME_OUT_ = 1;

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

    setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socket_option, sizeof(socket_option));

    if (bind(socket_descriptor, (struct sockaddr *)&address_, sizeof(address_))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    return socket_descriptor;
}

void Server::run() {
    std::vector<std::shared_ptr<ServerWorker>> currentWorkers;
    std::mutex workersMutex;
    std::condition_variable workersCondition;
    bool stop_manager_worker = false;

    pool.enqueue([&currentWorkers, &stop_manager_worker, &workersMutex]{
            using namespace std::chrono_literals;

            while(!stop_manager_worker) {
                {
                    std::unique_lock<std::mutex> lock(workersMutex);
                    
                    if(stop_manager_worker && currentWorkers.empty())
                        break;

                    for(auto it = currentWorkers.begin(); it != currentWorkers.end();) {
                        auto& worker = *it;
                        double elapsed_time = clock() - worker->lastUpdated();
                        if (elapsed_time >= 5000) {
                            worker->stop();
                            it = currentWorkers.erase(it);
                            std::cout << "MANAGER removed a worker" << std::endl;
                        } else {
                            ++it;
                        }
                    }
                }

                std::this_thread::sleep_for(10ms);

            }
            
            });


    int socket_descriptor = create_socket_descriptor();
    int address_len = sizeof(address_);
    if (listen(socket_descriptor, 3) < 0)
    {
        perror("Unable to listen using the socket");
        exit(EXIT_FAILURE);
    }
    // LINUX only
    // Setting timoeout for read/rcv socket calls.
    //struct timeval tv;
    //tv.tv_sec = 4;
    //tv.tv_usec = 0;
    //setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);


    while(true) {
        int new_socket_listener = accept(socket_descriptor, (struct sockaddr *)&address_, (socklen_t*)&address_len);
        if (new_socket_listener < 0) {
            perror("Unable to accept the connection"); 
            exit(EXIT_FAILURE);
        }

        std::shared_ptr<ServerWorker> worker = std::make_shared<ServerWorker>(new_socket_listener);

        std::cout << "New Request Received\n" << std::flush;

        pool.enqueue( [new_socket_listener, worker] {
            worker->processRequests();
        });

        pool.enqueue( [new_socket_listener, worker] {
            worker->readRequests();
        });
        {
            std::unique_lock<std::mutex> lock(workersMutex);
            currentWorkers.emplace_back(worker);
        }

    }

    // required to stop the manager worker.
    stop_manager_worker = true;
}
