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

    void printTrueLabelsPreWeights();

    void sortVectors();

    void build(FILE *fp);

    int startingNode;
    int epoca = 0;

    int rootsSize = 0;
    int rootsPos = 0;

    std::vector<int> roots;
#if PRINT_MAT
    std::vector<std::vector<int>> mat;
#endif

};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H