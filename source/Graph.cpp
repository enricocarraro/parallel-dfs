//
//  Graph.cpp
//  ParallelDFS
//
//  Created by Enrico on 09/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include "Graph.hpp"



using namespace std;

void Graph::init() {
    nodes.resize(nNodes);
    roots.reserve(nNodes);
    if(roots.size() > 0)
        return;
    for (int i = 0; i < nNodes; i++) {
        nodes[i].id = i;
        roots.insert(i);
    }
}

// should only be used by build
void Graph::build_addEdges(unsigned u, const unsigned adj[], unsigned adj_size)
{
    
    nodes[u].adj.resize(adj_size);
    for(int i = 0; i < adj_size; i++) {
        nodes[u].adj[i] = adj[i];
        nodes[adj[i]].inc.emplace(u, false);
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
    unsigned* buf = new unsigned(nNodes + 1);
    
    while(fscanf(fp, "%[^#]s", str) != EOF) {
        fscanf(fp, "%s", dontcare);
        char *token;
        unsigned i = 0;
        
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
            
            roots.erase(v);
            
            buf[i++] = v;
            token = strtok(NULL, " ");
        }
        
        this->build_addEdges(u, buf, i);
        
    }
    
}



Graph::Graph(FILE * fp): nodes(), roots(), P()
{
    fscanf(fp, "%d", &nNodes);
    init();
    build(fp);
    
}

Graph::Graph(unsigned nNodes): nodes(), roots(), P()
{
    this->nNodes = nNodes;
    init();
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

// meant to be used after building the graph.
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
    nodes[v].inc.emplace(u, false);
    
}

void Graph::buildDT() {
    queue<unsigned> Q;
    
    if(roots.size() <= 0) {
        cout << "Error: no roots" << endl;
        return;
    }
    
    for (const auto& x: roots) Q.push(x);
    
    
    
    while(Q.size() > 0) {
        // min(4,Q.size) -> running at most 4 threads in parallel
        std::vector<std::future<void> > bfs;
        bfs.reserve(Q.size());
        SafeQueue<unsigned> P;
        for(int i = 0; i < Q.size(); i++) {
            bfs.push_back(std::async(std::launch::async, &Graph::buildDT_processParent, this, Q.front()));
            Q.pop();
        }
        
        for(int i = 0; i < bfs.size(); i++)
            bfs[i].get();
        
        Q = P.move_underlying_queue();
    }
}


void Graph::buildDT_processParent(const unsigned p) {
    vector<future<void>> children;
    children.reserve(nodes[p].adj.size());
    for(int i = 0; i < nodes[p].adj.size(); i++)
        children.push_back(std::async(std::launch::async, &Graph::buildDT_processChildren, this, nodes[p].adj[i], p));
    
    for(int i = 0; i < children.size(); i++)
        children[i].get();
}
void Graph::buildDT_processChildren(unsigned child, unsigned p) {
    vector<unsigned> new_path(nodes[p].path);
    new_path.push_back(p);
    bool select_new_path = false;

    for(int i = 0; i < min(new_path.size(), nodes[child].path.size()); i++) {
        if(new_path[i] != nodes[child].path[i] ) {
            select_new_path = new_path[i] < nodes[child].path[i];
            break;
        }
    }
   
    if(nodes[child].path.size() == 0 || select_new_path)
        nodes[child].path = std::move(new_path);
    
    if(nodes[child].inc.find(p) == nodes[child].inc.end()) {
        throw string("Error, impossible, father must be in inc list");
    }
    nodes[child].inc[p] = true;
    
}

void Graph::computeSubGraphSize(){}
void Graph::computePrePostOrder(){}


