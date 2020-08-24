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
    int child;
};

class feederManager {
    vector<Worker> *workers;
    Semaphore *commonSemQueueFull;
    Semaphore *commonSemQueueEmpty;
    std::vector<struct intint> *commonQueue;
    int nWorkers;
    //Node *separator;
    int graphSize;
    Graph *g;
    Node terminator;

public:
    feederManager(vector<Worker> *allWorkers, int nWorkers,
                  Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                  std::vector<intint> *commonQueue,/* Node *separator,*/Graph *g); //commonQueue must have size equal to graphSize
    void feedLoop();
    void subGraphSize();
    void labels();
};

#endif //SDP_PIPELINERESOLUTION_FEEDERMANAGER_H
