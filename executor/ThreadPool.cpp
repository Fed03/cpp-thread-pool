#include "../taskQueue/TaskQueue.h"
#include <future>
#include <vector>
#include "ThreadPool.h"

void ThreadPool::threadFunc() {
    while (!done) {
        auto taskPtr = workQueue.waitAndPop();
        if (taskPtr != nullptr) {
            FuncWrapper task;
            task = std::move(*taskPtr);

            task();
        }
    }
}

ThreadPool::ThreadPool() : done(false) {
    const unsigned availableThreads = std::thread::hardware_concurrency();
    const unsigned threadsCount = std::max<unsigned>((availableThreads - 1), 1);
    try {
        for (unsigned i = 0; i < threadsCount; ++i) {
            threads.emplace_back(&threadFunc, this);
        }
    } catch(...) {
        done = true;
        throw;
    }
}

ThreadPool::~ThreadPool() {
    done=true;
    workQueue.invalidate();
    for (auto &thread : threads) {
        if(thread.joinable()) {
            thread.join();
        }
    }
}

template<typename FunctionType>
std::future<typename std::result_of<FunctionType()>::type> ThreadPool::submit(FunctionType function) {
    typedef typename std::result_of<FunctionType()>::type resultType;

    std::packaged_task<resultType()> task(std::move(function));
    std::future<resultType> future = task.get_future();

    workQueue.push(move(task));
    return future;
}