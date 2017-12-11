#include <iostream>
#include <thread>
#include <numeric>
#include <future>
#include "Executors.h"

#define BUFFER_SIZE 1000000
#define CHUNK_NUMBER 100
#define REPETITIONS 100

int accumulateBlock(int *buffer, int blockSize, int blockOffset) {
    int start = blockOffset * blockSize;
    int end = start + blockSize;
    return std::accumulate(buffer + start, buffer + end, 0);
}

void poolSolution(int *buffer, int blockSize) {
    ThreadPool& pool = Executors::newAutoThreadPool();

    std::future<int> futuresList[CHUNK_NUMBER];

    for (int k = 0; k < CHUNK_NUMBER; ++k) {
        futuresList[k] = pool.submit(std::bind(accumulateBlock, std::ref(buffer), blockSize, k));
    }

    int result = 0;
    for(auto &future : futuresList) {
        result += future.get();
    }
}

void spawnSolution(int *buffer, int blockSize) {
    std::future<int> futuresList[CHUNK_NUMBER];
    std::thread threads[CHUNK_NUMBER];

    for (int j = 0; j < CHUNK_NUMBER; ++j) {
        std::packaged_task<int()> task(std::bind(accumulateBlock, std::ref(buffer), blockSize, j));

        futuresList[j] = task.get_future();
        threads[j] = std::thread(std::move(task));
    }

    int result = 0;
    for(auto &future : futuresList) {
        result += future.get();
    }

    for(auto &thread: threads) {
        thread.join();
    }
}

int main() {
    int buffer[BUFFER_SIZE];
    for (long i = 0; i < BUFFER_SIZE; ++i) {
        buffer[i] = 1;
    }

    int blockSize = BUFFER_SIZE/CHUNK_NUMBER;

    auto duration = std::chrono::microseconds(0);
    for (long j = 0; j < REPETITIONS; ++j) {
        auto start = std::chrono::high_resolution_clock::now();
//        poolSolution(std::ref(buffer), blockSize);
        spawnSolution(std::ref(buffer), blockSize);
        auto end = std::chrono::high_resolution_clock::now();

        duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }

    auto avg = duration.count() / REPETITIONS;

    std::cout << avg << std::endl;
}