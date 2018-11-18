#include "../headers/Socket.h"
#include "../headers/Request.h"
#include <regex>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

const size_t Socket::BUFFER_SIZE = 1024;
const std::regex Socket::REQUEST_TERMINATOR("\\r\\n\\s*\\r\\n");

Socket::Socket(int socket_descriptor)
    : socket_descriptor_(socket_descriptor), last_updated_() {}

Socket::~Socket() { close(socket_descriptor_); }

/**
 * Reads a specific amount of bytes from the socket buffer.
 * @param length The number of bytes to be read from the socket buffer or
 * received from the client.
 * @return String containing the data that existed in the socket buffer, whose
 * length is equal to the specified length.
 */
std::string Socket::readInput(int length) {
    while (buffer_.length() < length) {
        char buffer[BUFFER_SIZE] = {0};
        read(socket_descriptor_, buffer, BUFFER_SIZE);
        std::string new_input(buffer);
        buffer_ += new_input;
        last_updated_ = clock();
    }
    std::string result = buffer_.substr(0, length);
    buffer_ = buffer_.substr(length, buffer_.length() - length);
    return result;
}

/**
 * Reads the next request by terminating just after reading a \r\n\s*\r\n.
 * @return Request object for the first request in the socket buffer.
 */
std::shared_ptr<Request> Socket::readNextRequest() {
    std::smatch matches;
    std::regex_search(buffer_, matches, REQUEST_TERMINATOR);
    while (matches.size() == 0) {
        char buffer[BUFFER_SIZE] = {0};

        int data_length = read(socket_descriptor_, buffer, BUFFER_SIZE);
        if (data_length == 0)
            return nullptr;
        std::string new_input(buffer, buffer + data_length);
        buffer_ += new_input;
        std::regex_search(buffer_, matches, REQUEST_TERMINATOR);
        last_updated_ = clock();
    }
    int break_point = matches[0].length() + matches.position(0);
    std::shared_ptr<Request> request =
        Request::build_request(buffer_.substr(0, break_point));
    buffer_ = buffer_.substr(break_point, buffer_.length() - break_point + 1);
    return request;
}

/**
 * Writes a string to the socket buffer to be sent to the client.
 * @param s String to be written to the client.
 */
void Socket::writeOutput(std::string s) {
    last_updated_ = clock();
    const char *out = s.c_str();
    for (size_t i = 0; i < s.length(); i += BUFFER_SIZE) {
        send(socket_descriptor_, out + i,
             std::min(BUFFER_SIZE, (int)s.length() - i), 0);
        last_updated_ = clock();
    }
}

/**
* @return last time the socket has been updates(read/write). 
*/
clock_t Socket::lastUpdated() { return last_updated_; }
