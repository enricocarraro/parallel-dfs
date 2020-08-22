#ifndef SDP_PIPELINERESOLUTION_GRAPHBUILDER_H
#define SDP_PIPELINERESOLUTION_GRAPHBUILDER_H

#include <string.h>
#include "Semaphore.h"
#include <iostream>
#include <sstream>
#include "Graph.h"

class GraphBuilder {

    Graph *g;
    int size;

public:
    std::string str;
    Semaphore free = Semaphore (1, 1);
    Semaphore working = Semaphore (0, 1);
    void initialize(Graph &g, int size);
    void buildLoop();
};

#endif //SDP_PIPELINERESOLUTION_GRAPHBUILDER_H
