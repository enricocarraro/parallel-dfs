#include "Graph.hpp"

using namespace std;

Graph::Graph(FILE *fp): bits() {
    fscanf(fp, "%d", &nNodes);
    nodes.resize(nNodes);
    bits.resize(nNodes);
    for (int i = 0; i < nNodes; i++) {
        nodes[i].id = i;
        bits.at(i) = false;
    }
    this->build(fp);
    
}

Graph::Graph(int nNodes): bits(), 
nNodes(nNodes) {
    nodes.resize(nNodes);
    for (int i = 0; i < nNodes; i++)
        nodes[i].id = i;
}

void Graph::printGraph() {
    for (int v = 0; v < nNodes; ++v) {
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        for (auto x : nodes[v].adj)
            cout << "-> " << x;
        printf("\n");
    }
}

void Graph::printTrueGraph() {
    for (int v = 0; v < nNodes; ++v) {
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        for (auto x : nodes[v].trueAdj)
            cout << "-> " << x;
        printf("\n");
    }
}

void Graph::sortVectors() {
    for (int v = 0; v < nNodes; ++v) {
        sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less<int>());
    }
}

void Graph::addEdge(int u, int v) {
    nodes[u].adj.push_back(v);
    bits.at(v) = true;
}

void Graph::addEdges(int u, const int adj[], int adj_size) {
    if (nodes[u].adj.size() == 0) {
        nodes[u].adj.resize(adj_size);
        for (int i = 0; i < adj_size; i++) {
            nodes[u].adj[i] = adj[i];
            bits.at(adj[i]) = true;
        }
    } else
        nodes[u].adj.insert(nodes[u].adj.end(), &adj[0], &adj[adj_size]);
}

void Graph::build(FILE *fp) {
    int u, v;
    unsigned max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char tmp[3];
    int buf[10000];
    
    while (fscanf(fp, "%[^#]s", str) != EOF) {
        fscanf(fp, "%s", tmp);
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
        while (token != NULL) {
#if GRAPH_DEBUG
            printf( " %s\n", token );
#endif
            sscanf(token, "%d", &v);
            buf[i++] = v;
            token = strtok(NULL, " ");
        }
        if (i > 0 && i < 10000) {
            this->addEdges(u, buf, i);
        }
        
    }
    
}

std::vector<bool> Graph::returnRoots() {
    return bits;
}
