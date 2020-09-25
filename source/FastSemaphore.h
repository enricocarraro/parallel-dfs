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
    Semaphore m_semaphore;
    int count;
};

#endif //SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H
