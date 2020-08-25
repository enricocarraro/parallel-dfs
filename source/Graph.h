//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_GRAPH_H
#define SDP_PIPELINERESOLUTION_GRAPH_H

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>

#define GRAPH_DEBUG 0

#ifndef INTINT
#define INTINT
struct intint {
    int father;
    int child;
};
#endif

struct intVet {
    int start;
    std::vector<int> *fathers;
};

struct Node {
    int id;
    int father = -1;
    int start = -1;
    int end = -1;
    int subTreeSize = 1;
    std::vector<int> *adj;
    std::vector<int> *trueAdj;
    int exitingArcs = 0;
    //std::vector<int> *ancestors;
};

class Graph {
    std::vector<bool> bits;
public:
    int nNodes;
    std::vector<Node> nodes;
    explicit Graph(FILE *fp);
    explicit Graph(int nodes);
    void addEdge(int u, int v);
    void build_addEdges(unsigned u, std::vector<unsigned>& adj, unsigned adj_size);
    void printGraph();
    void printTrueGraph();
    void printTrueGraphSize();
    void printTrueLabels();
    void sortVectors();
    void build(FILE *fp);
    std::vector<bool> returnRoots();
    int size() {
        return nNodes;
    }
    std::vector<bool> leaves;
    std::vector<intint> *cancelledEdges;
    std::vector<intVet> *st_father;
    std::vector<bool> *modified;
};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H