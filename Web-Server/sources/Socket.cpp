#include "../headers/Socket.h"
#include "../headers/Request.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <regex>

const size_t Socket::BUFFER_SIZE = 1024;
const std::regex Socket::REQUEST_TERMINATOR("\\r\\n\\s*\\r\\n");

Socket::Socket(int socket_descriptor): socket_descriptor_(socket_descriptor) {}

std::string Socket::readInput(int length) {
    while(buffer_.length() < length) {
        char buffer[BUFFER_SIZE] = {0};
        read(socket_descriptor_, buffer, BUFFER_SIZE);
        std::string new_input(buffer);
        buffer_ += new_input;
    }
    std::string result = buffer_.substr(0, length);
    buffer_ = buffer_.substr(length, buffer_.length() - length);
    return result;
}

Request* Socket::readNextRequest() {
    std::smatch matches;
    std::regex_search(buffer_, matches, REQUEST_TERMINATOR);
    while(matches.size() == 0) {
        char buffer[BUFFER_SIZE] = {0};
        read(socket_descriptor_, buffer, BUFFER_SIZE);
        std::string new_input(buffer);
        buffer_ += new_input;
        std::regex_search(buffer_, matches, REQUEST_TERMINATOR);
    }
    int break_point = matches[0].length() + matches.position(0);
    Request* request = Request::build_request(buffer_.substr(0, break_point));
    buffer_ = buffer_.substr(break_point, buffer_.length() - break_point + 1);
    return request;
}

void Socket::writeOutput(std::string s) {
    const char* out = s.c_str();
    for(size_t i = 0; i < s.length(); i += BUFFER_SIZE) {
        send(socket_descriptor_, out + i, std::min(BUFFER_SIZE, (int)s.length() - i), 0);
    }
}