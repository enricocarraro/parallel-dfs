#ifndef SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H
#define SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H

#include "Semaphore.h"
#include <mutex>
#include <atomic>
#include <condition_variable>
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
    int count;
    std::mutex mux;
    std::condition_variable cv;
};

#endif //SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H
