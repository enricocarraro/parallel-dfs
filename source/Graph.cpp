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

Graph::Graph(unsigned nNodes): nNodes(nNodes)
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

void Graph::sortVectors()
{
    for (int v = 0; v < nNodes; ++v)
    {
        sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less<int>());
    }
}

void Graph::addEdge(unsigned u, unsigned v)
{
    nodes[u].adj.push_back(v);
}


void Graph::addEdges(unsigned u, const unsigned adj[], unsigned adj_size)
{
    if(nodes[u].adj.size() == 0) {
        nodes[u].adj.resize(adj_size);
        for(int i = 0; i < adj_size; i++)
            nodes[u].adj[i] = adj[i];
    }
    else
        nodes[u].adj.insert(nodes[u].adj.end(), &adj[0], &adj[adj_size]);
}

void Graph::build(FILE * fp) {
    unsigned u, v;
    unsigned max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char dontcare[3];
    vector<unsigned> buf(nNodes);
    
    while(fscanf(fp, "%[^#]s", str) != EOF) {
        fscanf(fp, "%s", dontcare);
        char *token;
        int i = 0;
        
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
            buf[i++] = v;
            token = strtok(NULL, " ");
        }
        if(i > 0 && i < 10000) {
            this->addEdges(u, buf.data(), i);
        }
        
    }
    
}
