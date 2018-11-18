#ifndef WEB_SERVER_SERVER_H
#define WEB_SERVER_SERVER_H

#include <netinet/in.h>
#include "../headers/ThreadPool.h"

class Server {

public:
    Server();
    Server(int port);
    void run();

private:
    const static int DEFAULT_PORT_;
    const static int THREADS_COUNT_;
    const static int READ_TIME_OUT_;
    ThreadPool pool;

    int port_;
    struct sockaddr_in address_;

    void configure_address();
    int create_socket_descriptor();

};


#endif //WEB_SERVER_SERVER_H
