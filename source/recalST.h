//
// Created by lire on 8/25/20.
//

#ifndef SDP_PIPELINERESOLUTION_RECALST_H
#define SDP_PIPELINERESOLUTION_RECALST_H

#include "Graph.h"

class recalST {
    void iterFather(int father, int child);
public:
    Graph *g;
    void init(Graph *g);
    void work();
};

#endif //SDP_PIPELINERESOLUTION_RECALST_H
