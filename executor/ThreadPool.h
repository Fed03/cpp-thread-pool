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

    template <typename FunctionType>
    using funcResultType = typename std::result_of_t<FunctionType()>;

public:
    explicit ThreadPool() : done(false) {
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

    ~ThreadPool() {
        done=true;
        workQueue.invalidate();
        for (auto &thread : threads) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    };

    template <typename FunctionType>
    std::future<funcResultType<FunctionType>> submit(FunctionType function) {
        typedef funcResultType<FunctionType> resultType;

        std::packaged_task<resultType()> task(std::move(function));
        std::future<resultType> future = task.get_future();

        workQueue.push(move(task));
        return future;
    };

    template <typename FunctionType>
    std::vector<std::future<funcResultType<FunctionType>>> invokeAll(const std::vector<FunctionType>& funcList) {
        typedef std::future<funcResultType<FunctionType>> resultType;

        std::vector<resultType> results;
        for (const auto &func : funcList) {
            results.push_back(submit(func));
        }

        return results;
    }
};


#endif //THREAD_POOL_THREADPOOL_H
