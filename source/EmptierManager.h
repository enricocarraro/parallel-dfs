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
    std::vector<struct intint> *commonQueue;
    int nWorkers;
    //Node *separator;
    int graphSize;
    Graph *g;
    std::vector<bool> graph;
    std::vector<bool> roots;

public:
    emptierManager(std::vector<Worker> *allWorkers, int nWorkers,
                   Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                   std::vector<intint> *commonQueue,/* Node *separator,*/
                    Graph *g); //commonQueue must have size equal to graphSize
    void pushLoop();
    void subGraphSize();
    void labels();
};

#endif //SDP_PIPELINERESOLUTION_EMPTIERMANAGER_H