#include <iostream>
#include <thread>
#include <numeric>
#include <future>

#define BUFFER_SIZE 10000
#define NUM_THREADS 10

int main() {
    int buffer[BUFFER_SIZE];
    for (long i = 0; i < BUFFER_SIZE; ++i) {
        buffer[i] = 1;
    }

    std::future<int> futures[NUM_THREADS];
    int blockSize = BUFFER_SIZE/NUM_THREADS;

    auto start = std::chrono::high_resolution_clock::now();

    std::thread threads[NUM_THREADS];
    for (int j = 0; j < NUM_THREADS; ++j) {
        std::packaged_task<int()> task([&buffer, j, blockSize] {
            int start = j*blockSize;
            int end = start + blockSize;
            return std::accumulate(buffer + start, buffer + end, 0);
        });

        futures[j] = task.get_future();
        threads[j] = std::thread(std::move(task));
    }

    int result = 0;
    for(auto &future : futures) {
        result += future.get();
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end- start).count();
    std::cout<< duration << std::endl;

    for(auto &thread: threads) {
        thread.join();
    }
//    ThreadPool& pool = Executors::newAutoThreadPool();
}