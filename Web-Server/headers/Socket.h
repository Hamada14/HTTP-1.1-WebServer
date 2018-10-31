#ifndef WEB_SERVER_SOCKET_H
#define WEB_SERVER_SOCKET_H


#include <string>
#include <regex>
#include "Request.h"

class Socket {
public:
    Socket(int socket_descriptor);
    std::string readInput(int length);
    Request* readNextRequest();
    void writeOutput(std::string s);

private:
    const static size_t BUFFER_SIZE;
    const static std::regex REQUEST_TERMINATOR;

    int socket_descriptor_;
    std::string buffer_;
};


#endif //WEB_SERVER_SOCKET_H
