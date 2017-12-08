#ifndef THREAD_POOL_THREADPOOL_H
#define THREAD_POOL_THREADPOOL_H

#include <vector>
#include <future>
#include "../Task.h"
#include "../taskQueue/TaskQueue.h"

class ThreadPool {
    std::atomic_bool done;
    TaskQueue<FuncWrapper> workQueue;
    std::vector<std::thread> threads;

    void threadFunc();

public:
    ThreadPool();
    virtual ~ThreadPool();

    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType function);
};


#endif //THREAD_POOL_THREADPOOL_H
