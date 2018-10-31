#ifndef WEB_SERVER_POSTREQUEST_H
#define WEB_SERVER_POSTREQUEST_H

#include "Request.h"
#include <string>
#include "Socket.h"

class PostRequest : public Request {
public:
    PostRequest(std::string request);
    void process(Socket* socket);
};


#endif //WEB_SERVER_POSTREQUEST_H
