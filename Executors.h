#ifndef THREAD_POOL_EXECUTORS_H
#define THREAD_POOL_EXECUTORS_H

#include "executor/ThreadPool.h"

class Executors {
public:
    static ThreadPool& newAutoThreadPool() {
        static ThreadPool pool;
        return pool;
    }
private:
    Executors() = delete;
    Executors(const Executors&) = delete;
    Executors& operator=(const Executors&) = delete;
};

#endif //THREAD_POOL_EXECUTORS_H
