#include <iostream>
#include "executor/ThreadPool.h"

int main() {
    ThreadPool pool;

    std::future<int> res1 = pool.submit([] {
        std::cout << std::this_thread::get_id() << std::endl;
        return 5;
    });

    std::future<int> res2 = pool.submit([] {
        std::cout << std::this_thread::get_id() << std::endl;
        return 2;
    });

    int r = res1.get() + res2.get();
    std::cout << r << std::endl;
}