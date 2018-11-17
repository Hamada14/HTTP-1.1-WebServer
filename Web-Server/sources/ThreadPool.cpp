#include "../headers/ThreadPool.h"

#include <iostream>
ThreadPool::ThreadPool(size_t threadCount):
    threadCount(threadCount) {

    for (size_t i = 0; i < threadCount; ++i) {
        workers.emplace_back([this] {
                while(true)
                {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(tasksMutex);

                        condition.wait(lock, [this]{return isStopping || !tasks.empty();});
                        if (isStopping && tasks.empty())
                            break;

                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
    }

}

void ThreadPool::enqueue(const Task& task)
{
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::stop() {
    if (isStopping)
        return;
    {
        // CRITICAL SECTION
        std::unique_lock<std::mutex> lock(tasksMutex);
        isStopping = true;
    }
    
    condition.notify_all();


    for (auto &t: workers) {
        t.join();
    }
}


