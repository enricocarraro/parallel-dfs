//
//  SafeQueue.cpp
//  parallel-dfs
//
//  Created by Enrico on 18/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include "SafeQueue.hpp"

#if WITH_CONDVAR

template <class T>
SafeQueue<T>::SafeQueue(void): q(), m(), c() {}

// Add an element to the queue.
template <class T>
void SafeQueue<T>::enqueue(T t)
{
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    c.notify_one();
}

// Get the "front"-element.
// If the queue is empty, wait till a element is avaiable.
template <class T>
T SafeQueue<T>::dequeue(void) {
    std::unique_lock<std::mutex> lock(m);
    while(q.empty())
    {
        // release lock as long as the wait and reaquire it afterwards.
        c.wait(lock);
    }
    T val = q.front();
    q.pop();
    return val;
}

#else

template<class T>
void SafeQueue<T>::push(T elem) {
    std::lock_guard<std::mutex> lock(m);
    q.push(elem);
}

template<class T>
bool SafeQueue<T>::pop(T& elem) {
    std::lock_guard<std::mutex> lock(m);
    if (q.empty()) {
        return false;
    }
    elem = q.front();
    q.pop();
    return true;
}


#endif
