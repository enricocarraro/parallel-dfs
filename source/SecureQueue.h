//
// Created by lire on 10/3/20.
//

#ifndef SDP_PIPELINERESOLUTION_SECUREQUEUE_H
#define SDP_PIPELINERESOLUTION_SECUREQUEUE_H

#include <vector>

#include <atomic>
#include "FastSemaphore.h"
#include "BusySemaphore.h"

#define OPT_ATOMIC 0

class SecureQueue {
    int queueSize;
public:
#if OPT_ATOMIC
    std::vector<bool> barrier = {true};
    std::atomic<int> insertPosition;
    std::atomic<int> extractPosition;
#else
    int insertPosition;
    int extractPosition;
#endif
    std::vector<int> queue;
    FastSemaphore dataReady;
    BusySemaphore bs;

    SecureQueue(int queueSize, int nWorker);
    int push(int next);
    int pop();
    void reset();
};


#endif //SDP_PIPELINERESOLUTION_SECUREQUEUE_H
