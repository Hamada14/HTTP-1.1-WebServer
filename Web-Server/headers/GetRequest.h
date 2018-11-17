#ifndef WEB_SERVER_GETREQUEST_H
#define WEB_SERVER_GETREQUEST_H

#include "Request.h"
#include <string>
#include <regex>

class GetRequest : public Request {
public:
    GetRequest(std::string request, std::map<std::string, std::string> headers);
    void process(Socket* socket);

private:
    const static std::regex FILE_PATH_REGEX;
    const static char BACK_SLASH;
    std::string file_path_;
    std::string extract_file_path(std::string request);
    std::map<std::string, std::string> headers;
};


#endif //WEB_SERVER_GETREQUEST_H
