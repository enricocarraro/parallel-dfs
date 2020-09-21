//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_EMPTIERMANAGER_H
#define SDP_PIPELINERESOLUTION_EMPTIERMANAGER_H

#include <queue>
#include <vector>

#include "FeederManager.h"
#include "Worker.h"

class emptierManager {
    std::vector<Worker> *workers;
    Semaphore *commonSemQueueFull;
    Semaphore *commonSemQueueEmpty;
    std::vector<struct intintint> *commonQueue;
    int nWorkers;
    //Node *separator;
    int graphSize;
    int nEdges;
    Graph *g;
    //std::vector<bool> graph;
#if USE_BOOL
    std::vector<bool> *roots;
#else
    std::vector<int> *roots;
    std::vector<int> *preLeaves;
#endif

public:
    emptierManager(std::vector<Worker> *allWorkers, int nWorkers,
                   Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                   std::vector<intintint> *commonQueue,/* Node *separator,*/
                    Graph *g); //commonQueue must have size equal to graphSize
#if !QUICK_TIME_CALC
    void pushLoop();
    void subGraphSize();
    void labels();
#else
    //new implementation
    void weightsAndPrefixes();
    void startEndTimes();
#endif
    void preGraphSize();
};

#endif //SDP_PIPELINERESOLUTION_EMPTIERMANAGER_H