#ifndef Graph_hpp
#define Graph_hpp


#include "SafeQueue.hpp"
#include "ThreadWorker.hpp"
#include "Semaphore.hpp"
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
    bool visited = false;
    vector<unsigned> path;
    
};

class Graph
{
    unsigned nNodes, pre, post;
    vector<mutex> muxes;
    vector<Node> nodes;
    vector<ThreadWorker> workers;
    unordered_set<unsigned> roots;
    SafeQueue<unsigned> P, processParentQ;
    SafeQueue<std::pair<unsigned, unsigned>>processChildQ;
    void init();
    void build(FILE * fp);
    void build_addEdges(unsigned u, vector<unsigned>& adj, unsigned adj_size);
    void buildDT_processParent(const unsigned p);
    void buildDT_processChild(unsigned i, unsigned p);
    
    void sequentialDFS_r(unsigned p);
    unsigned int hash(unsigned int x);
public:
    Graph(FILE * fp);
    Graph(unsigned nodes);
    void addEdge(unsigned u, unsigned v);
    void printGraph();
    void printNodesStatus();
    void sortVectors();
    void sequentialDFS();
    void buildDT();
    void computeSubGraphSize();
    void computePrePostOrder();
};


#endif /* Graph_hpp */

