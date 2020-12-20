//
//  FastSemaphore.cpp
//  parallel-dfs
//
//  Created by Enrico on 13/09/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include "FastSemaphore.hpp"

FastSemaphore::FastSemaphore() noexcept : m_count(0), m_semaphore(0) {}

void FastSemaphore::signal()
{
        std::atomic_thread_fence(std::memory_order_release);
        int count = m_count.fetch_add(1, std::memory_order_relaxed);
        if (count < 0)
                m_semaphore.signal();
}

void FastSemaphore::wait()
{
        int count = m_count.fetch_sub(1, std::memory_order_relaxed);
        if (count < 1)
                m_semaphore.wait();
        std::atomic_thread_fence(std::memory_order_acquire);
}
