project(sdp_parallelSolution)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -O2")
add_executable(sdp_parallelSolution main.cpp Worker.cpp Worker.h Graph.cpp Graph.h OPTIONS.h Node.h)


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
#set(Boost_INCLUDE_DIR /usr/local/include/boost)
#set(Boost_LIBRARY_DIR /usr/local/lib)

#find_package(Boost COMPONENTS system filesystem REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIR})

#add_executable(sdp_parallelSolution main.cpp Worker.cpp Worker.h Graph.cpp Graph.h Semaphore.cpp Semaphore.h FastSemaphore.cpp FastSemaphore.h OPTIONS.h SecureQueue.cpp SecureQueue.h BusySemaphore.cpp BusySemaphore.h Node.h)
#target_link_libraries( sdp_parallelSolution ${Boost_LIBRARIES} )