#ifndef WEB_SERVER_POSTREQUEST_H
#define WEB_SERVER_POSTREQUEST_H

#include "Request.h"
#include <string>
#include "Socket.h"

class PostRequest : public Request {
public:
    PostRequest(std::string request, std::map<std::string, std::string> headers);
    void process(Socket* socket);

private:
    const static std::regex FILE_PATH_REGEX;

    std::map<std::string, std::string> headers_;
    std::string file_path_;

    std::string extract_file_path(std::string request);
};


#endif //WEB_SERVER_POSTREQUEST_H
