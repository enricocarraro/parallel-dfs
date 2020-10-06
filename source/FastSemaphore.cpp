#include "FastSemaphore.h"

FastSemaphore::FastSemaphore() noexcept: m_count(0) { count = 0; }
FastSemaphore::FastSemaphore(int count) noexcept: m_count(count) { this->count = count; }

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

void FastSemaphore::reset()
{
    m_count = count;
}