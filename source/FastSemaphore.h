#ifndef SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H
#define SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H

#include "OPTIONS.h"

#if !SEM_INDEPENDENT
#include "Semaphore.h"
#else
#include <condition_variable>
#include <mutex>
#endif
#include <atomic>
#include <cassert>

class FastSemaphore
{
public:
    FastSemaphore() noexcept;
    FastSemaphore(int count) noexcept;
    void signal();
    void wait();
    void reset();

private:
    std::atomic<int> m_count;
#if !SEM_INDEPENDENT
    Semaphore m_semaphore;
#else
    std::mutex mux;
    std::condition_variable cv;
#endif
    int count;
};

#endif //SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H
