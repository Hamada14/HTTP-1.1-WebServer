#ifndef WEB_SERVER_SOCKET_H
#define WEB_SERVER_SOCKET_H


#include <string>
#include <regex>
#include <ctime>
#include <memory>
#include "Request.h"

class Socket {
public:
    Socket(int socket_descriptor);
    ~Socket();
    std::string readInput(int length);
    std::shared_ptr<Request> readNextRequest();
    void writeOutput(std::string s);
    clock_t lastUpdated();

private:
    const static size_t BUFFER_SIZE;
    const static std::regex REQUEST_TERMINATOR;

    int socket_descriptor_;
    std::string buffer_;
    std::clock_t last_updated_;

};


#endif //WEB_SERVER_SOCKET_H
