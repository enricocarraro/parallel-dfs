//
//  Graph.hpp
//  ParallelDFS
//
//  Created by Enrico on 09/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

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
    int nNodes;
    vector<Node> nodes;
public:
    Graph(int nEdges);
    void addEdge(int u, int v);
    void printGraph();
    void sortVectors();
    void build(ifstream InFile);
};


#endif /* Graph_hpp */
