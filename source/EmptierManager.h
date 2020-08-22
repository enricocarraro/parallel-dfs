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
    vector<Worker> *workers;
    Semaphore *commonSemQueueFull;
    Semaphore *commonSemQueueEmpty;
    int queueInsertPosition = 0;
    std::vector<struct intint> *commonQueue;
    int nWorkers;
    //Node *separator;
    int graphSize;
    std::vector<bool> graph;
    int nodeRead = 0;

public:
    emptierManager(vector<Worker> *allWorkers, int nWorkers,
                   Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                   std::vector<intint> *commonQueue,/* Node *separator,*/
                   int graphSize); //commonQueue must have size equal to graphSize
    void pushLoop(std::vector<bool> roots);
};

#endif //SDP_PIPELINERESOLUTION_EMPTIERMANAGER_H