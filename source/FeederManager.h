//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_FEEDERMANAGER_H
#define SDP_PIPELINERESOLUTION_FEEDERMANAGER_H

#include <queue>
#include <vector>
#include "Graph.h"
#include "Worker.h"

#ifndef INTINT
#define INTINT
struct intint {
    int father;
    int child;
};
#endif

class feederManager {
    std::vector<Worker> *workers;
    Semaphore *commonSemQueueFull;
    Semaphore *commonSemQueueEmpty;
    std::vector<struct intint> *commonQueue;
    int nWorkers;
    //Node *separator;
    int graphSize;
    Graph *g;
    Node terminator;

public:
    feederManager(std::vector<Worker> *allWorkers, int nWorkers,
                  Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                  std::vector<intint> *commonQueue,/* Node *separator,*/Graph *g); //commonQueue must have size equal to graphSize
    void feedLoop();
    void subGraphSize();
    void labels();
};

#endif //SDP_PIPELINERESOLUTION_FEEDERMANAGER_H
