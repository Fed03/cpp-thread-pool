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
    std::condition_variable checkData;

    std::unique_ptr<node> head;
    node* tail;

    std::atomic_bool isValid;

    node* getTail(){
        std::lock_guard<std::mutex> tailLock(tailMtx);
        return tail;
    }

public:
    TaskQueue() : head(new node), tail(head.get()), isValid(true) {}
    TaskQueue(const TaskQueue& other) = delete;
    TaskQueue& operator=(const TaskQueue& other)= delete;

    bool empty() {
        std::lock_guard<std::mutex> headLock(headMtx);

        return head.get() == getTail();
    }

    void push(T newValue){
        std::unique_ptr<node> newTail(new node);
        node* newTailPtr = newTail.get();
        std::shared_ptr<T> data = std::make_shared<T>(std::move(newValue));
        {
            std::lock_guard<std::mutex> lock(tailMtx);
            tail->data = data;
            tail->next = std::move(newTail);
            tail = newTailPtr;
        }
        checkData.notify_one();
    }

    std::shared_ptr<T> waitAndPop(){
        std::unique_lock<std::mutex> headLock(headMtx);
        checkData.wait(headLock, [this] {
            return (head.get() != getTail()) || !isValid;
        });

        if(!isValid) {
            return nullptr;
        }

        std::unique_ptr<node> item = std::move(head);
        head = std::move(item->next);
        return item->data;
    }

    void invalidate() {
        std::lock_guard<std::mutex> headLk(headMtx);
        std::lock_guard<std::mutex> tailLk(tailMtx);
        isValid = false;
        checkData.notify_all();
    }
};


#endif //THREAD_POOL_TASKQUEUE_H
