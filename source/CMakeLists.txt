project(sdp_pipelineReSolution)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread" )

add_executable(sdp_pipelineSolution main.cpp FeederManager.cpp FeederManager.h EmptierManager.cpp EmptierManager.h Worker.cpp Worker.h Graph.h Graph.cpp Semaphore.cpp Semaphore.h Timer.cpp GraphBuilder.cpp GraphBuilder.h Node.h)