//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_GRAPH_H
#define SDP_PIPELINERESOLUTION_GRAPH_H

#include <stdio.h>
#include <string.h>
//#include <vector>
#include <iostream>
//#include <algorithm>
#include <fstream>
#include <cmath>

#include "OPTIONS.h"
#include <atomic>
//#include "FastSemaphore.h"
#include "Node.h"



class Graph {


public:

    int nNodes;
    //int nEdges = 0;
    std::vector<Node> nodes;

    explicit Graph(FILE *fp);

    explicit Graph(int nodes);

    void build_addEdges(unsigned u, std::vector<unsigned> &adj, unsigned adj_size);

    void printTrueLabelsPreWeights();

    void sortVectors();

    void build(FILE *fp);



    void reBuild();


    //int preLeavesSize;
    int preLeavesPos = 0;
    int rootsSize = 0;
    int rootsPos = 0;

    std::atomic<int> posIntoPreLeaves = 0;
    std::vector<int> preLeaves;
    //std::atomic<int> posIntoRoots = 0;
    std::vector<int> roots;

};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H