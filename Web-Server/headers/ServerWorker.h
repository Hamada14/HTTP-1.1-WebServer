#ifndef WEB_SERVER_SERVERWORKER_H
#define WEB_SERVER_SERVERWORKER_H

#include "Socket.h"

class ServerWorker {

public:
    ServerWorker(int socket_identifier);
    void process();

private:
    Socket socket;
};


#endif //WEB_SERVER_SERVERWORKER_H
