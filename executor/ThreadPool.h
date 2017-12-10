#ifndef THREAD_POOL_THREADPOOL_H
#define THREAD_POOL_THREADPOOL_H

#include <vector>
#include <future>
#include "../FunctionWrapper.h"
#include "../ConcurrentQueue.h"

class ThreadPool {
    std::atomic_bool done;
    ConcurrentQueue<FuncWrapper> workQueue;
    std::vector<std::thread> threads;

    void threadFunc() {
        while (!done) {
            auto taskPtr = workQueue.waitAndPop();
            if (taskPtr != nullptr) {
                FuncWrapper task;
                task = std::move(*taskPtr);

                task();
            }
        }
    };

public:
    ThreadPool() : done(false) {
        const unsigned availableThreads = std::thread::hardware_concurrency();
        const unsigned threadsCount = std::max<unsigned>((availableThreads - 1), 1);
        try {
            for (unsigned i = 0; i < threadsCount; ++i) {
                threads.emplace_back(&ThreadPool::threadFunc, this);
            }
        } catch(...) {
            done = true;
            throw;
        }
    };
    virtual ~ThreadPool() {
        done=true;
        workQueue.invalidate();
        for (auto &thread : threads) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    };

    template <typename FunctionType>
    std::future<typename std::result_of_t<FunctionType()>> submit(FunctionType function) {
        typedef typename std::result_of_t<FunctionType()> resultType;

        std::packaged_task<resultType()> task(std::move(function));
        std::future<resultType> future = task.get_future();

        workQueue.push(move(task));
        return future;
    };
};


#endif //THREAD_POOL_THREADPOOL_H
