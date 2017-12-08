#include <iostream>
#include "executor/ThreadPool.h"

int ros(int f) {
    return 5+f;
}

int main() {
    ThreadPool pool;

    std::future<int> res1 = pool.submit([] { return ros(3); });

    std::future<int> res2 = pool.submit([] {
        return 2;
    });

    int r = res1.get() + res2.get();
    std::cout << r << std::endl;
}