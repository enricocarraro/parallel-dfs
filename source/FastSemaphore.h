#ifndef SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H
#define SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H

#include "OPTIONS.h"

#include "Semaphore.h"
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
    Semaphore m_semaphore;
    int count;
};

#endif //SDP_PIPELINERESOLUTION_FASTSEMAPHORE_H
