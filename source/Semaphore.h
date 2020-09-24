//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_SEMAPHORE_H
#define SDP_PIPELINERESOLUTION_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

class Semaphore {
    std::mutex mux;
    std::condition_variable cv;
    int initialCount;
    int count;
    int maxCount;

public:
    void wait();

    void signal();

    void reset(); //use with extreme caution, reset 'count' to initial state
    Semaphore(int count,
              int maxCount); //maxCount is the max value the semaphore can assume (binary semaphore if maxCount = 1)
    Semaphore(int count); //maxCount = INT_MAX
    Semaphore(); //count=0 maxCount=INT_MAX
};

#endif //SDP_PIPELINERESOLUTION_SEMAPHORE_H
