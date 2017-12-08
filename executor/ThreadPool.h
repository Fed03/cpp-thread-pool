#ifndef THREAD_POOL_THREADPOOL_H
#define THREAD_POOL_THREADPOOL_H

//template <typename FunctionType>
//typedef typename std::result_of<FunctionType()>::type FuncReturnType;
//typedef typename std::packaged_task<FuncReturnType()> TaskType;

#include <vector>
#include <future>
#include "../Task.h"
#include "../taskQueue/TaskQueue.h"

class ThreadPool {
    TaskQueue<FuncWrapper> workQueue;
    std::atomic_bool done;
    std::vector<std::thread> threads;

    void threadFunc(){
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
    }

    virtual ~ThreadPool() {
        done=true;
        workQueue.invalidate();
        for (auto &thread : threads) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    }

    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType function) {
        typedef typename std::result_of<FunctionType()>::type resultType;

        std::packaged_task<resultType()> task(std::move(function));
        std::future<resultType> future = task.get_future();

        workQueue.push(std::move(task));
        return future;
    }
};


#endif //THREAD_POOL_THREADPOOL_H
