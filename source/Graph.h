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

using namespace std;

struct Node {
    int id;
    int father = -1;
    int start = -1;
    int end = -1;
    int subTreeSize = -1;
    vector<int> adj;
    vector<int> trueAdj;
};

class Graph {
    int nNodes;
    std::vector<bool> bits;
public:
    vector<Node> nodes;
    explicit Graph(FILE *fp);
    explicit Graph(int nodes);
    void addEdge(int u, int v);
    void build_addEdges(unsigned u, vector<unsigned>& adj, unsigned adj_size);
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
