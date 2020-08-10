//
//  Graph.cpp
//  ParallelDFS
//
//  Created by Enrico on 09/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include "Graph.hpp"

using namespace std;


Graph::Graph(FILE * fp)
{
    fscanf(fp, "%d", &nNodes);
    nodes.resize(nNodes);
    for (int i = 0; i < nNodes; i++)
        nodes[i].id = i;
    this->build(fp);
    
}

Graph::Graph(int nNodes): nNodes(nNodes)
{
    nodes.resize(nNodes);
    for (int i = 0; i < nNodes; i++)
        nodes[i].id = i;
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

/*void Graph::sortVectors()
{
    for (int v = 0; v < nNodes; ++v)
    {
        sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less<int>());
    }
}*/

void Graph::addEdge(int u, int v)
{
    nodes[u].adj.insert(v);
}

void Graph::build(FILE * fp) {
    int u, v;
    char str[500];
    char tmp[3];
    while(fscanf(fp, "%[^#]s", str) != EOF) {
        fscanf(fp, "%s", tmp);
        char *token;
        
        /* get the first token */
        token = strtok(str, " ");
#if GRAPH_DEBUG
        printf( " %s\n", token );
#endif
        sscanf(token, "%d", &u);
        token = strtok(NULL, " ");
        /* walk through other tokens */
        while(token != NULL){
#if GRAPH_DEBUG
            printf( " %s\n", token );
#endif
            sscanf(token, "%d", &v);
            this->addEdge(u, v);
            token = strtok(NULL, " ");
        }
        
    }
    
}
