#ifndef ThreadPool_hpp
#define ThreadPool_hpp

#include <functional>
#include <queue>
#include <thread>
#include <condition_variable>


class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(size_t thread_count);

    ~ThreadPool();

    void enqueue(const Task& task);
    void stop();

private:
    size_t threadCount;
    std::vector<std::thread> workers;
    std::queue<Task> tasks;

    std::mutex tasksMutex;
    std::condition_variable condition;

    bool isStopping = false;

};
#endif // ifndef ThreadPool_hpp
