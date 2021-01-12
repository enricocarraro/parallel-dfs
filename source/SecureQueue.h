//
// Created by lire on 10/3/20.
//

#ifndef SDP_PIPELINERESOLUTION_SECUREQUEUE_H
#define SDP_PIPELINERESOLUTION_SECUREQUEUE_H

#include <vector>

#include <atomic>
#include "FastSemaphore.h"


class SecureQueue {
    int queueSize;
    std::vector<char> control;
    std::vector<int> controlN;
    bool ready = true;
public:
    std::atomic<int> insertPosition;
    std::atomic<int> extractPosition;
    std::vector<int> queue;
    FastSemaphore dataReady;

    SecureQueue(int queueSize, int nWorker);
    int push(int next);
    int pop();
    void reset();
};


#endif //SDP_PIPELINERESOLUTION_SECUREQUEUE_H
