//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_GRAPH_H
#define SDP_PIPELINERESOLUTION_GRAPH_H

#include <iostream>
#include <cmath>
#include <algorithm>

#include "OPTIONS.h"
#include "Node.h"


class Graph {


public:

    int nNodes;

    std::vector<Node> nodes;

    explicit Graph(FILE *fp);

    void build_addEdges(unsigned u, std::vector<unsigned> &adj, unsigned adj_size);

    void sortVectors();

#if READ_TYPE == 0
    void build(FILE *fp);
#elif READ_TYPE == 1
    void build2(FILE *fp);
#elif READ_TYPE == 2
    void build3(FILE *fp);
#endif

    //int startingNode;
    Node* startNode;

    int rootsSize = 0;
    int rootsPos = 0;

    std::vector<int> roots;

    Node terminator;

};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H