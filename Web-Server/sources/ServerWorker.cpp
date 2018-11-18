#include "../headers/ServerWorker.h"
#include <iostream>

ServerWorker::ServerWorker(int socket_identifier): socket_(socket_identifier){
}


void ServerWorker::readRequests() {

    while(true) {

        {
            std::cout << "Reader is looping" << std::endl;
            if (requests_has_post_) {
                std::unique_lock<std::mutex> lock(requests_mutex_);
                std::cout << "Reader Waiting" << std::endl;
                
                requests_condition_.wait(lock, 
                        [this](){return this->is_stopping_ || !this->requests_has_post_;});

            }
            if (is_stopping_)
                break;

            std::cout << "Reader done Waiting" << std::endl;

            auto request = socket_.readNextRequest();
            if (request->type() == Request::Type::EMPTY)
                continue;
            std::cout << "Another Request" << std::endl;

            {
                std::unique_lock<std::mutex> lock(requests_mutex_);

                requests_.push(request);

                if (request->type() == Request::Type::POST)
                    this->requests_has_post_ = true;
            }
            requests_condition_.notify_all();
        }
    }
    std::cout << "Reader is done baby" << std::endl;

}

void ServerWorker::processRequests() {
    while(true) {
        std::cout << "Writer is looping" << std::endl;
        std::shared_ptr<Request> request;
        {
            std::unique_lock<std::mutex> lock(requests_mutex_);

            std::cout << "writer Waiting" << std::endl;
            requests_condition_.wait(lock,
                    [this](){return this->is_stopping_ || !this->requests_.empty();});

            if(is_stopping_ && requests_.empty())
                break;

            std::cout << "writer done Waiting" << std::endl;
            request = requests_.front();
            requests_.pop();
        }

        request->process(&this->socket_);

        if (request->type() == Request::Type::POST)
        {
            requests_has_post_ = false;
            requests_condition_.notify_one();
        
        }
    }
    std::cout << "Writer is done baby" << std::endl;
}

clock_t ServerWorker::lastUpdated() {
    return socket_.lastUpdated();
}

void ServerWorker::stop() {
    is_stopping_ = true;
}
