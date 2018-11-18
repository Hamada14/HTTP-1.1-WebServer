#ifndef WEB_SERVER_SERVER_H
#define WEB_SERVER_SERVER_H

#include "../headers/ServerWorker.h"
#include "../headers/ThreadPool.h"
#include <netinet/in.h>
#include <vector>

class Server {

  public:
    Server();
    Server(int port);
    ~Server();
    void run();
    void startManager();
    void stop();

  private:
    // Default Port used to listen for connections.
    const static int DEFAULT_PORT_;
    // Number of threads used in the thread pool.
    const static int THREADS_COUNT_;
    // Time in millis if no updates from an open connection.
    const static int CONNECTION_TIME_OUT_;
    // Maximum allowed of connected clients to serve.
    const static int MAX_ALLOWED_CLIENTS_;

    int port_;
    struct sockaddr_in address_;

    void configure_address();
    int create_socket_descriptor();

    ThreadPool pool_;

    // Keeps track of current ServerWorkers handling each client.
    std::vector<std::shared_ptr<ServerWorker>> current_workers_;
    std::mutex workers_mutex_;

    bool is_stopping_ = false;
};

#endif // WEB_SERVER_SERVER_H
