//
//  Graph.hpp
//  ParallelDFS
//
//  Created by Enrico on 09/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <unordered_set>

#define GRAPH_DEBUG 0

using namespace std;


struct Node
{
    int id;
    int father = -1;
    int start = -1;
    int end = -1;
    int subTreeSize = -1;
    vector<int> adj;
};

class Graph
{
    unsigned nNodes;
    vector<Node> nodes;
    unordered_set<unsigned> roots;
    void init();
    void build(FILE * fp);
    void addEdges_build(unsigned u, const unsigned adj[], unsigned adj_size);
public:
    Graph(FILE * fp);
    Graph(unsigned nodes);
    void addEdge(unsigned u, unsigned v);
    void printGraph();
    void sortVectors();
    void makeDT();
    void computeSubGraphSize();
    void computePrePostOrder();
};


#endif /* Graph_hpp */

