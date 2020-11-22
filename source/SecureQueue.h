//
// Created by lire on 10/3/20.
//

#ifndef SDP_PIPELINERESOLUTION_SECUREQUEUE_H
#define SDP_PIPELINERESOLUTION_SECUREQUEUE_H

#include <vector>

#include <atomic>
#include "FastSemaphore.h"

#define OPT_ATOMIC 0

class SecureQueue {
    int queueSize;
    std::vector<char> control;
    std::vector<int> controlN;
    bool ready = true;
public:
#if OPT_ATOMIC
    std::vector<bool> barrier = {true};
    std::atomic<int> insertPosition;
    std::atomic<int> extractPosition;
#else
    std::atomic<int> insertPosition;
    std::atomic<int> extractPosition;
#endif
    std::vector<int> queue;
    FastSemaphore dataReady;

    SecureQueue(int queueSize, int nWorker);
    int push(int next);
    int pop();
    void reset();
};


#endif //SDP_PIPELINERESOLUTION_SECUREQUEUE_H
