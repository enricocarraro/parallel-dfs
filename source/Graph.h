#ifndef SDP_PIPELINERESOLUTION_GRAPH_H
#define SDP_PIPELINERESOLUTION_GRAPH_H

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <thread>
#include "Node.h"
#include <cmath>

#define GRAPH_DEBUG 0
#define GraphBuilderN 3

class Graph {
    int nNodes;
    std::vector<bool> bits;
    //void startBuilders(GraphBuilder *gb);
public:
    std::vector<Node> nodes;
    explicit Graph(FILE *fp);
    //explicit Graph(int nodes);
    void addEdge(int u, int v);
    void addEdges(int u, const int adj[], int adj_size);
    void addEdges(int u, std::vector<int> adj, int adj_size);
    void printGraph();
    void printTrueGraph();
    void sortVectors();
    void build(FILE *fp);
    std::vector<bool> returnRoots();
    int size() {
        return nNodes;
    }
};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H
