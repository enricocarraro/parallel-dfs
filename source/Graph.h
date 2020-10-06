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
#include <boost/multiprecision/cpp_int.hpp>

#include "OPTIONS.h"
#include <atomic>
#include "FastSemaphore.h"
#include "BusySemaphore.h"

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
    boost::multiprecision::uint1024_t nodeWeight = 1;
    boost::multiprecision::uint1024_t time = std::numeric_limits<boost::multiprecision::uint1024_t>::max();

    int start = INT32_MAX;
    int end = -1;

    int subTreeSize = 1;

    std::vector<int> *adj;
    int adjSize = 0;

    std::vector<int> *ancestors;
    int ancSize = 0;

#if GRAPH_DOUBLE_READ | GRAPH_REREAD_GRAPH
    int ancNumber = 0;
#endif
#if !USE_BOOL
    bool root = true;
#endif

    bool operator<(const Node &n) const {
        return (nodeWeight < n.nodeWeight);
    }

    FastSemaphore *sem = new FastSemaphore(1);
    std::mutex *mux = new std::mutex;
    BusySemaphore *bSem = new BusySemaphore;
};

class Graph {


public:

    int nNodes;
    int nEdges = 0;
    std::vector<Node> nodes;

    explicit Graph(FILE *fp);

    explicit Graph(int nodes);

    void addEdge(int u, int v);

    void build_addEdges(unsigned u, std::vector<unsigned> &adj, unsigned adj_size);

    void printGraph();

    void printTrueGraph();

    void printTrueGraphSize();

    void printTrueLabels();

    void printTrueLabelsPreWeights();

    void sortVectors();

    void build(FILE *fp);


    //dimensione sovrastimata del vettore per calcolare il preorder
    boost::multiprecision::uint1024_t preorderVetSize = 0;
    std::vector<int> preorder;


#if GRAPH_DOUBLE_READ | GRAPH_REREAD_GRAPH

    void reBuild(FILE *fp);

#endif
#if GRAPH_DOUBLE_READ
    void addAncestor(unsigned u, std::vector<unsigned>& adj, unsigned adj_size);
#endif

    //std::vector<bool> returnRoots();
#if USE_BOOL
    std::vector<bool> roots;
    std::vector<bool> leaves;
    std::vector<bool> preLeaves;
#else
    //int preLeavesSize;
    int preLeavesPos = 0;
    int rootsSize = 0;
    int rootsPos = 0;

    std::atomic<int> posIntoPreLeaves = 0;
    std::vector<int> preLeaves;
    std::atomic<int> posIntoRoots = 0;
    std::vector<int> roots;

#endif
};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H