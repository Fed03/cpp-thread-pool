#include "TaskQueue.h"

template<typename T>
void TaskQueue<T>::push(T newValue) {
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

template<typename T>
std::shared_ptr<T> TaskQueue<T>::waitAndPop() {
    std::unique_lock<std::mutex> headLock(headMtx);
    checkData.wait(headLock, [this] {
        return head.get() != getTail();
    });
    std::unique_ptr<node> item = std::move(head);
    head = std::move(item->next);
    return item->data;
}

template<typename T>
bool TaskQueue<T>::empty() {
    std::lock_guard<std::mutex> headLock(headMtx);

    return head.get() == getTail();
}

template<typename T>
typename TaskQueue<T>::node* TaskQueue<T>::getTail() {
    std::lock_guard<std::mutex> tailLock(tailMtx);
    return tail;
}
