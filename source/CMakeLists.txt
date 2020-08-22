project(PathBasedThreadWorkers)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3" )

add_executable(PathBasedThreadWorkers main.cpp Semaphore.cpp Semaphore.hpp Timer.cpp SafeQueue.cpp SafeQueue.hpp ThreadWorker.cpp ThreadWorker.hpp Graph.cpp Graph.hpp)
