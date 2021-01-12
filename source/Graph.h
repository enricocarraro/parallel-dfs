//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_GRAPH_H
#define SDP_PIPELINERESOLUTION_GRAPH_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cmath>

#include "OPTIONS.h"
#include <atomic>
#include "Node.h"



class Graph {


public:

    int nNodes;

    std::vector<Node> nodes;

    explicit Graph(FILE *fp);

    explicit Graph(int nodes);

    void build_addEdges(unsigned u, std::vector<unsigned> &adj, unsigned adj_size);

    void printTrueLabelsPreWeights();

    void sortVectors();
#if !EXPERIMENTAL_READ
    void build(FILE *fp);
#else
#if READ_TYPE == 0
    void build1(FILE *fp);
#elif READ_TYPE == 1
    void build2(FILE *fp);
#elif READ_TYPE == 2
    void build3(FILE *fp);
#endif
#endif

    void reBuild();

    int preLeavesPos = 0;
    int rootsSize = 0;
    int rootsPos = 0;

    std::atomic<int> posIntoPreLeaves = 0;
    std::vector<int> preLeaves;
    std::vector<int> roots;

};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H