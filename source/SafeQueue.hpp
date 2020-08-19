//
//  SafeQueue.hpp
//  parallel-dfs
//
//  Created by Enrico on 18/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#ifndef SafeQueue_hpp
#define SafeQueue_hpp

#include <queue>
#include <mutex>
#include <condition_variable>


template <typename T>
class SafeQueue
{

    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
public:
    SafeQueue();
// Add an element to the queue.
    void push(T t);
    size_t size();
// Get the "front"-element.
// If the queue is empty, wait till a element is avaiable.
    T pop(void);
    std::queue<T> move_underlying_queue();
};

/*
template<typename T>
class SafeQueue {

    std::queue<T> q;
    std::mutex m;

public:

    void push(T elem) {
        std::lock_guard<std::mutex> lock(m);
        q.push(elem);
    }

    bool pop(T elem) {
        std::lock_guard<std::mutex> lock(m);
        if (q.empty()) {
            return false;
        }
        elem = q.front();
        q.pop();
        return true;
    }

};

*/
#endif /* SafeQueue_hpp */
