//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_WORKER_H
#define SDP_PIPELINERESOLUTION_WORKER_H

//#include <queue>
#include <vector>

#include "Graph.h"
#include "OPTIONS.h"
#include "SecureQueue.h"


struct sharedData {
    Graph *g;
    SecureQueue sq;
    std::vector<std::atomic<int>> visitedNeighbours;
    std::atomic<int> nodeRead = 0;
    sharedData(Graph *gra, int nWorker): g(gra), sq(gra->nNodes, nWorker), visitedNeighbours(gra->nNodes) {
        for(int i=0; i<gra->nNodes; i++) {
            visitedNeighbours.at(i) = 0;
        }
    };
};


class Worker {


public:

    //Node *separator;

    Graph *g;
    int graphSize;

    sharedData *sd;


    void initialize(sharedData *sd);

    void nodeSize();

    void nodeWeights(bool works_on_roots);

    void nodeTimes();
};

#endif //SDP_PIPELINERESOLUTION_WORKER_H
