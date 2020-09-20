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

#define GRAPH_DEBUG 0

#define GRAPH_OPT 1
#define GRAPH_PUSHBACK 0
#define GRAPH_DOUBLE_READ 0
#define GRAPH_REREAD_GRAPH 1
#define USE_BOOL 0

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
    //unsigned long int fatherWeight = UINT64_MAX;
    boost::multiprecision::uint1024_t fatherWeight = std::numeric_limits<boost::multiprecision::uint1024_t>::max(), descendantSize = 1;
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
    //unsigned long int descendantSize = 1;
#if GRAPH_DOUBLE_READ | GRAPH_REREAD_GRAPH
    int ancNumber = 0;
#endif
#if !USE_BOOL
    bool root = true;
    bool leaf = true;
    bool preleaf = true;
#endif
};

class Graph {

    //ottimizzazione salvataggio archi cancellati
    int maxCancelled = 0;
public:
    //vettore pesi
    //std::vector<int> nodesWeights;

    //__int128_t var;
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
    //std::vector<int> leaves; //not sure if this helps
    std::vector<bool> leaves; //keeping this, for now
    std::vector<int> preLeaves;
    std::vector<int> roots;
#endif
    std::vector<intVet> *st_father;
    std::vector<bool> *modified;
};

#endif //SDP_PIPELINERESOLUTION_GRAPH_H