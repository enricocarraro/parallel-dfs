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
    nodes.reserve(nNodes);
    roots.reserve(nNodes);
    init(nNodes);
    this->build(fp);
    
}

Graph::Graph(unsigned nNodes): nNodes(nNodes)
{
    nodes.reserve(nNodes);
    roots.reserve(nNodes);
    init(nNodes);
}

void Graph::init(unsigned nNodes) {
    if(roots.size() > 0)
        return;
    for (int i = 0; i < nNodes; i++) {
        nodes[i].id = i;
        roots.insert(i);
    }
}


void Graph::printGraph()
{
   
    
    for (int v = 0; v < nNodes; ++v)
    {
   
            cout << "\n Adjacency list of vertex " << v << "\n head ";

        for (auto x : nodes[v].adj)
            cout << "-> " << x;
        cout << endl;

            
    }
    
    cout << "Roots (" << roots.size() << "): " << endl;
       for ( const auto& x: roots )
           cout << x << " ";
       cout << endl;
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

// should only be used by build
void Graph::addEdges_build(unsigned u, const unsigned adj[], unsigned adj_size)
{

    nodes[u].adj.resize(adj_size);
    for(int i = 0; i < adj_size; i++) {
        nodes[u].adj[i] = adj[i];
    }
}


/*
 Presuming the graph input file is well formed:
 max_size = (log10(nNodes) + 2) * (nNodes + 1) + 3
 log10(nNodes) = max characters to represent nodeId
 + 1 for the intermitting space between edges
 + 1 for slack
 * nNodes = maxmimumg amount of nodes in adj list
 + includes the node to which the list refers to
 + 3 slack
 */

void Graph::build(FILE * fp) {
    unsigned u, v;
    unsigned max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char dontcare[3];
    unsigned *buf = new unsigned(nNodes);
    
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
            
            if(roots.find(v) != roots.end())
                roots.erase(v);
            
            buf[i++] = v;
            token = strtok(NULL, " ");
        }
        if(i > 0 && i < 10000) {
            this->addEdges_build(u, buf, i);
        }
        
    }
    
}
