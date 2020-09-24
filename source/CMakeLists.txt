project(sdp_pipelineReSolution)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O3")
set(Boost_INCLUDE_DIR /usr/local/include/boost)
set(Boost_LIBRARY_DIR /usr/local/lib)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(sdp_pipelineSolution main.cpp FeederManager.cpp FeederManager.h EmptierManager.cpp EmptierManager.h Worker.cpp Worker.h Graph.cpp Graph.h Semaphore.cpp Semaphore.h)
