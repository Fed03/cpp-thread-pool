cmake_minimum_required(VERSION 3.9)
project(thread_pool)

set(CMAKE_CXX_STANDARD 11)

add_executable(thread_pool main.cpp taskQueue/TaskQueue.h executor/ThreadPool.h Task.h)