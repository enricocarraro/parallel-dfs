#ifndef Graph_hpp
#define Graph_hpp

#include "SafeQueue.hpp"
#include "ThreadWorker.hpp"
#include "Semaphore.hpp"
#include "FastSemaphore.hpp"
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
#include <climits>
#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;
using namespace std;

#define GRAPH_DEBUG 1

struct Node
{
        unsigned int id,
            inc_visited_count = 0,
            adj_visited_count = 0,
            lab_visited_count = 0,
            prefix_subgraph_size = 0,
            pre = 0,
            depth = 0,
            s = numeric_limits<unsigned int>::max(),
            e = 0;
            

        int parent = -1;
        uint1024_t cost = std::numeric_limits<uint1024_t>::max(), subgraph_size = 1, post = 0;
        vector<unsigned int> adj, inc;
        bool visited = false;
};

class Graph
{
        unsigned int nNodes, pre, post;
        bool init_tw_done = false;
        vector<mutex> muxes;
        vector<Node> nodes;
        vector<ThreadWorker> parent_workers;
        vector<ThreadWorker> child_workers;
        vector<FastSemaphore> worker_semaphores;
        unordered_set<unsigned int> roots;
        vector<unsigned int> roots_sorted;
        unordered_set<unsigned int> leafs;
        vector<unsigned int> o_leafs;
        SafeQueue<unsigned int> P, C;
        void init();
        void build(FILE *fp);
        void build_addEdges(unsigned int u, vector<unsigned int> &adj, unsigned int adj_size);
        void subGraphSize_computePrefixSum(unsigned int p);
        void subGraphSize_processChild(unsigned int i, unsigned int worker_id);
        void subGraphSize_processParent(unsigned int p, unsigned int i);
        void subDTSize_processChild(unsigned int i, unsigned int worker_id);
        void subDTSize_processParent(unsigned int p, unsigned int i);
        void computeParentSSSP_processParent(unsigned int p, unsigned int worker_id);
        void computeParentSSSP_processChild(unsigned int p, unsigned int i);
        void sequentialDFS_r(unsigned int p);
        unsigned int hash(unsigned int x);
        void initThreadWorkers();

public:
        Graph(FILE *fp);
        Graph(unsigned int nodes);
        void addEdge(unsigned int u, unsigned int v);
        void printGraph();
        void printNodesStatus();
        void sortVectors();
        void sequentialDFS();
        void computeParentSSSP();
        void computeSubGraphSize();
        void computeSubDTSize();
        void computePrePostOrder();
        void computeLabels();
};

#endif /* Graph_hpp */
