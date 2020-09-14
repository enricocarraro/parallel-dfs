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

#define GRAPH_OPT 1
#define GRAPH_PUSHBACK 0
#define GRAPH_DOUBLE_READ 0
#define GRAPH_REREAD_GRAPH 1

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
    int fatherWeight = INT32_MAX;
    //int newFatherWeight;
    int start = -1;
    int end = -1;
    int subTreeSize = 1;
    std::vector<int> *adj;
    int adjSize = 0;
    std::vector<int> *trueAdj;
    int exitingArcs = 0;
    std::vector<int> *ancestors;
    int ancSize = 0;
    int descendantSize = 1;
#if GRAPH_DOUBLE_READ | GRAPH_REREAD_GRAPH
    int ancNumber = 0;
#endif
};

class Graph {

    //ottimizzazione salvataggio archi cancellati
    int maxCancelled = 0;
public:
    //vettore pesi
    //std::vector<int> nodesWeights;

    std::vector<bool> roots;
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
    void printTrueLabelsPreWeights();
    void sortVectors();
    void build(FILE *fp);

    //ottimizzazione salvataggio archi cancellati
    std::vector<intint> *cancelledEdges;
    int posIntoCancelledEdges = 0;


#if GRAPH_DOUBLE_READ | GRAPH_REREAD_GRAPH
    void reBuild(FILE *fp);
#endif
#if GRAPH_DOUBLE_READ
    void addAncestor(unsigned u, std::vector<unsigned>& adj, unsigned adj_size);
#endif

    std::vector<bool> returnRoots();
    std::vector<bool> leaves;
    std::vector<bool> preLeaves;
    std::vector<intVet> *st_father;
    std::vector<bool> *modified;
};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H