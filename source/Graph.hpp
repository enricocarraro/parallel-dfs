#ifndef Graph_hpp
#define Graph_hpp


#include "SafeQueue.cpp"
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <future>
#include <vector>
#include <future>
#include <algorithm>
#include <chrono>
#include <random>
#include <mutex>

#define GRAPH_DEBUG 0

using namespace std;


struct Node
{
    unsigned id, inc_visited_count = 0;
    int parent = -1, start = -1, end = -1, subTreeSize= -1;
    vector<unsigned > adj;
    unordered_map<unsigned, bool> inc;
    /*vector<unsigned> inc;
    vector<bool> inc_visited;
     */
    bool no_path = true;
    vector<unsigned> path;
    mutex mux;
};

class Graph
{
    unsigned nNodes;
    vector<mutex> muxes;
    vector<Node> nodes;
    unordered_set<unsigned> roots;
    SafeQueue<unsigned> P;
    void init();
    void build(FILE * fp);
    void build_addEdges(unsigned u, vector<unsigned>& adj, unsigned adj_size);
    void buildDT_processParent(const unsigned p);
    void buildDT_processChildren(unsigned i, unsigned p);
public:
    Graph(FILE * fp);
    Graph(unsigned nodes);
    void addEdge(unsigned u, unsigned v);
    void printGraph();
    void printNodesStatus();
    void sortVectors();
    void buildDT();
    void computeSubGraphSize();
    void computePrePostOrder();
};


#endif /* Graph_hpp */

