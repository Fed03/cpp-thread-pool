#ifndef THREAD_POOL_THREADPOOL_H
#define THREAD_POOL_THREADPOOL_H

#include <vector>
#include <future>
#include <exception>
#include "../FunctionWrapper.h"
#include "../ConcurrentQueue.h"

class ThreadPool {
    std::atomic_bool done;
    std::atomic_bool shutdownFlag;
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

    void checkShutdownFlag() {
        if (shutdownFlag) {
            throw std::invalid_argument("Cannot submit more tasks. The pool has been shutted down.");
        }
    }

    void cleanupPool() {
        done=true;
        workQueue.invalidate();
    }

    template <typename FunctionType>
    using funcResultType = typename std::result_of_t<FunctionType()>;

public:
    explicit ThreadPool() : done(false), shutdownFlag(false) {
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
        cleanupPool();
        for (auto &thread : threads) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    };

    template <typename FunctionType>
    std::future<funcResultType<FunctionType>> submit(FunctionType function) {
        checkShutdownFlag();

        typedef funcResultType<FunctionType> resultType;

        std::packaged_task<resultType()> task(std::move(function));
        std::future<resultType> future = task.get_future();

        workQueue.push(move(task));
        return future;
    };

    template <typename FunctionType>
    std::vector<std::future<funcResultType<FunctionType>>> invokeAll(const std::vector<FunctionType>& funcList) {
        checkShutdownFlag();

        typedef std::future<funcResultType<FunctionType>> resultType;

        std::vector<resultType> results;
        for (const auto &func : funcList) {
            results.push_back(submit(func));
        }

        return results;
    }

    void shutdown() {
        shutdownFlag = true;
        cleanupPool();
    }

    bool isShutdown() {
        return shutdownFlag;
    }
};


#endif //THREAD_POOL_THREADPOOL_H
