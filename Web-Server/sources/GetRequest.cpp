#include "../headers/GetRequest.h"
#include "../headers/Socket.h"
#include "../headers/Config.h"
#include "../headers/Util.h"
#include <regex>
#include <fstream>
#include <iostream>

const std::regex GetRequest::FILE_PATH_REGEX("GET\\s(.+)\\sHTTP\\/1.1");
const char GetRequest::BACK_SLASH = '/';

/**
 * Constructor for the GetRequest class.
 * It extends the parent class "Request", where it presents the requests of type GET.
 * @param request String that contains the request.
 * @param headers Map containing the headers and their corresponding values.
 */
GetRequest::GetRequest(std::string request, std::map<std::string, std::string> headers): headers(headers) {
    file_path_ = Util::join_path(
            Config::getInstance()->get(Config::PUBLIC_DIRECTORY_KEY),
            extract_file_path(request));
    std::cout << file_path_ << std::endl;
}

/**
 * Process the request by attempting to read the file required and writing it to the socket.
 * In case the file doesn't exist, it'll return a 404.
 * @param socket
 */
void GetRequest::process(Socket *socket) {
    std::ifstream f_stream(file_path_.c_str(), std::ios::in | std::ios::binary);
    if(f_stream.good() && file_path_[file_path_.length() - 1] != BACK_SLASH) {
        std::string file_extension = "";
        if(file_path_.find(".") != std::string::npos) {
            file_extension = file_path_.substr(file_path_.find(".") + 1);
        }
        socket->writeOutput(read_file_response(f_stream, file_extension));
    } else {
        socket->writeOutput(not_found_response());
    }
}

/**
 * Retrieves the file name from the GET request.
 * @param request GET request received.
 * @return Name of the file required to be sent to the client.
 */
std::string GetRequest::extract_file_path(std::string request) {
    std::smatch matches;
    std::regex_search(request, matches, FILE_PATH_REGEX);
    return matches[1];
}