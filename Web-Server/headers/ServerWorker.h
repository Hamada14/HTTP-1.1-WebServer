#ifndef WEB_SERVER_SERVERWORKER_H
#define WEB_SERVER_SERVERWORKER_H

#include "Socket.h"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

class ServerWorker {

  public:
    ServerWorker(int socket_identifier);
    void readRequests();
    void processRequests();
    clock_t lastUpdated();
    void stop();

  private:
    Socket socket_;
    bool is_stopping_ = false;
    std::queue<std::shared_ptr<Request>> requests_;
    std::condition_variable requests_condition_;
    std::mutex requests_mutex_;
    bool requests_has_post_ = false;
};

#endif // WEB_SERVER_SERVERWORKER_H
