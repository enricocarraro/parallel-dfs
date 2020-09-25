//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_FEEDERMANAGER_H
#define SDP_PIPELINERESOLUTION_FEEDERMANAGER_H

#include <queue>
#include <vector>
#include "Graph.h"
#include "Worker.h"

#include "OPTIONS.h"


struct intintint {
    int start;
    int child;
    boost::multiprecision::uint1024_t prefix;
};

class feederManager {
    std::vector<Worker> *workers;
#if !USE_QUICK_SEM
    Semaphore *commonSemQueueFull;
    Semaphore *commonSemQueueEmpty;
#else
    FastSemaphore *commonSemQueueFull;
    FastSemaphore *commonSemQueueEmpty;
#endif
    std::vector<struct intintint> *commonQueue;
    int nWorkers;
    int graphSize;
    Graph *g;
    Node terminator;

public:
#if !USE_QUICK_SEM
    feederManager(std::vector<Worker> *allWorkers, int nWorkers,
                  Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                  std::vector<intintint> *commonQueue,/* Node *separator,*/
                  Graph *g); //commonQueue must have size equal to graphSize
#else
    feederManager(std::vector<Worker> *allWorkers, int nWorkers,
                  FastSemaphore *commonSemQueueFull, FastSemaphore *commonSemQueueEmpty,
                  std::vector<intintint> *commonQueue,/* Node *separator,*/
                  Graph *g); //commonQueue must have size equal to graphSize
#endif


    void weightsAndPrefixes();
    void startEndTimes();
    void preGraphSize();

};

#endif //SDP_PIPELINERESOLUTION_FEEDERMANAGER_H
