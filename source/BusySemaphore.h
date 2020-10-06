//
// Created by lire on 10/4/20.
//

#ifndef SDP_PIPELINERESOLUTION_BUSYSEMAPHORE_H
#define SDP_PIPELINERESOLUTION_BUSYSEMAPHORE_H


#include <atomic>

class BusySemaphore
{
public:
    BusySemaphore() noexcept;
    void signal();
    void wait();

private:
    std::atomic<bool> lock;
};

#endif //SDP_PIPELINERESOLUTION_BUSYSEMAPHORE_H
