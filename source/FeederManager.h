//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_FEEDERMANAGER_H
#define SDP_PIPELINERESOLUTION_FEEDERMANAGER_H

#include <queue>
#include <vector>
#include "Graph.h"
#include "Worker.h"

struct intint {
    int father;
    int son;
};

class feederManager {
    std::vector<Worker> *workers;
    Semaphore *commonSemQueueFull;
    Semaphore *commonSemQueueEmpty;
    int queueExtractPosition = 0;
    std::vector<struct intint> *commonQueue;
    int nWorkers;
    Node *separator;
    int graphSize;
    Graph *g;
    int nodeRead = 0;

public:
    feederManager(std::vector<Worker> *allWorkers, int nWorkers,
                  Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                  std::vector<intint> *commonQueue,/* Node *separator,*/Graph *g); //commonQueue must have size equal to graphSize
    void feedLoop();
};

#endif //SDP_PIPELINERESOLUTION_FEEDERMANAGER_H
