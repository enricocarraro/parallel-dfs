//
//  SafeQueue.cpp
//  parallel-dfs
//
//  Created by Enrico on 18/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include "SafeQueue.hpp"


template <typename T>
SafeQueue<T>::SafeQueue(): q(), m(), c() {}

template <typename T>
size_t SafeQueue<T>::size() {
    return q.size();
}
// Add an element to the queue.
template <typename T>
void SafeQueue<T>::push(T t)
{
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    c.notify_one();
}

template <typename T>
std::queue<T> SafeQueue<T>::move_underlying_queue() {
    return q;
}
// Get the "front"-element.
// If the queue is empty, wait till a element is avaiable.
template <typename T>
T SafeQueue<T>::pop() {
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
 
 /*

template<typename T>
void SafeQueue<T>::push(T elem) {
    std::lock_guard<std::mutex> lock(m);
    q.push(elem);
}

template<typename T>
bool SafeQueue<T>::pop(T& elem) {
    std::lock_guard<std::mutex> lock(m);
    if (q.empty()) {
        return false;
    }
    elem = q.front();
    q.pop();
    return true;
}


*/
