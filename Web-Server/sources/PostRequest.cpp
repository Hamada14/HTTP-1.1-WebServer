#include "../headers/PostRequest.h"
#include "../headers/Config.h"
#include "../headers/Util.h"
#include <fstream>


const std::regex PostRequest::FILE_PATH_REGEX("POST\\s(.+)\\sHTTP\\/1.1");

PostRequest::PostRequest(std::string request, std::map<std::string, std::string> headers): headers_(headers) {
    file_path_ = Util::join_path(
                Config::getInstance()->get(Config::PUBLIC_DIRECTORY_KEY), extract_file_path(request)
            );

}

void PostRequest::process(Socket *socket) {
    std::string response = build_response(Request::SUCCESS_RESPONSE_STATUS, Request::HTML_MIME_TYPE, "");
    socket->writeOutput(response);
    std::string file_content = socket->readInput(std::stoi(headers_[Request::CONTENT_LENGTH_HEADER]));
    std::ofstream f_stream(file_path_.c_str());
    f_stream.write(file_content.c_str(), file_content.length());
}



std::string PostRequest::extract_file_path(std::string request) {
    std::smatch matches;
    std::regex_search(request, matches, FILE_PATH_REGEX);
    return matches[1];
}