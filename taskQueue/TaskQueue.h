#ifndef THREAD_POOL_TASKQUEUE_H
#define THREAD_POOL_TASKQUEUE_H

#include <mutex>

template<typename T>
class TaskQueue {
private:
    struct node {
        std::unique_ptr<node> next;
        std::shared_ptr<T> data;
    };
    std::mutex headMtx;
    std::mutex tailMtx;
    std::unique_ptr<node> head;
    node* tail;
    std::condition_variable checkData;
    node* getTail();

public:
    TaskQueue() : head(new node), tail(head.get()) {}
    TaskQueue(const TaskQueue& other) = delete;
    TaskQueue& operator=(const TaskQueue& other)= delete;

    bool empty();
    void push(T newValue);
    std::shared_ptr<T> waitAndPop();
};


#endif //THREAD_POOL_TASKQUEUE_H
