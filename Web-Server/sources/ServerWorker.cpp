#include "../headers/ServerWorker.h"
#include <iostream>

ServerWorker::ServerWorker(int socket_identifier)
    : socket_(socket_identifier) {}

void ServerWorker::readRequests() {

	std::cout << "READER start\n" << std::flush;
    while (!is_stopping_) {
        auto request = socket_.readNextRequest();
        // Ignore if is an empty request.
        if (request == nullptr || request->type() == Request::Type::EMPTY)
            continue;

        // Ignore if request is not POST nor GET
        if (request->type() == Request::Type::UNDEFINED) {
            std::cout << "READER Received an undefined request\n" << std::flush;
            continue;
        }

        {
            // CRITICAL SECTION START
            std::unique_lock<std::mutex> lock(requests_mutex_);
            std::cout << "READER Received a new request\n" << std::flush;

            // add the request to the queue so writer can process it.
            requests_.push(request);

            if (request->type() == Request::Type::POST) {
                /*
                        If the current request is a POST then we can't read any
                   more requests as when the WRITER processes that request, it
                   is going to expect an ACK from the client, so it will have to
                   do a read. Until that POST request is processed, the READER
                   stays idle.
                */

                this->requests_has_post_ = true;

                std::cout << "READER Waiting for pos request to be processed\n"
                          << std::flush;

                requests_condition_.wait(
                    lock, [this]() { return !this->requests_has_post_; });

                std::cout << "READER post request is processed\n" << std::flush;
            }
            // CRITICAL SECTION END
        }
        requests_condition_.notify_all();
    }
    std::cout << "READER exit\n" << std::flush;
}

void ServerWorker::processRequests() {
	std::cout << "WRITER start\n" << std::flush;
    while (true) {
        std::shared_ptr<Request> request;
        {
            // CRITICAL SECTION START
            std::unique_lock<std::mutex> lock(requests_mutex_);

            std::cout << "WRITER Waiting for new requests" << std::endl;
            // Waits until MANAGER stops the Worker or until a new request is
            // received.
            requests_condition_.wait(lock, [this]() {
                return this->is_stopping_ || !this->requests_.empty();
            });

            if (is_stopping_ && requests_.empty())
                break;

            std::cout << "WRITER processing request" << std::endl;
            request = requests_.front();
            requests_.pop();
            // CRITICAL SECTION END
        }

        // Process the request
        request->process(&this->socket_);

        if (request->type() == Request::Type::POST) {
            // Notify the READER as it is waiting for the POST request to be
            // processed.
            requests_has_post_ = false;
            requests_condition_.notify_one();
        }
    }
    std::cout << "WRITER exit" << std::endl;
}

clock_t ServerWorker::lastUpdated() { return socket_.lastUpdated(); }

void ServerWorker::stop() {
    {
        // CRITICAL SECTION START
        std::unique_lock<std::mutex> lock(requests_mutex_);
        is_stopping_ = true;
        // CRITICAL SECTION END
    }
    requests_condition_.notify_all();
}
