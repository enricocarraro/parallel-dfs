//
//  Graph.cpp
//  ParallelDFS
//
//  Created by Enrico on 09/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include "Graph.hpp"



using namespace std;


Graph::Graph(int nNodes)
{
    this->nNodes = nNodes;
    nodes.resize(nNodes);
    for (int i = 0; i < nNodes; i++)
    {
        nodes[i].id = i;
    }
}

void Graph::printGraph()
{
    for (int v = 0; v < nNodes; ++v)
    {
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        for (auto x : nodes[v].adj)
            cout << "-> " << x;
        printf("\n");
    }
}

void Graph::sortVectors()
{
    for (int v = 0; v < nNodes; ++v)
    {
        sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less<int>());
    }
}

void Graph::addEdge(int u, int v)
{
    nodes[u].adj.push_back(v);
}

void Graph::build(ifstream InFile) {
    int u, v;
    
    while (InFile >> u >> v) this->addEdge(u, v);
    InFile.close();
}
