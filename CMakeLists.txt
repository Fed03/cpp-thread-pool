cmake_minimum_required(VERSION 3.9)
project(thread_pool)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(thread_pool main.cpp ConcurrentQueue.h executor/ThreadPool.h FunctionWrapper.h Executors.h)