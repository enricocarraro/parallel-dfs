//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_WORKER_H
#define SDP_PIPELINERESOLUTION_WORKER_H

#include <vector>

#include "Graph.h"
#include "OPTIONS.h"
//#include "SecureQueue.h"


class Worker {


public:

    Graph *g;

    int start(int position);

    void initialize(Graph *g);

};

#endif //SDP_PIPELINERESOLUTION_WORKER_H
