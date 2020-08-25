//
// Created by lire on 8/19/20.
//

#include "Graph.h"

using namespace std;

Graph::Graph(FILE *fp) {
    fscanf(fp, "%d", &nNodes);
    nodes.resize(nNodes);
    bits.resize(nNodes);
    leaves.resize(nNodes);
    cancelledEdges = new vector<intint>;
    st_father = new vector<intVet> (nNodes);
    modified = new vector<bool> (nNodes, false);
    for (int i = 0; i < nNodes; i++) {
        nodes[i].id = i;
        nodes[i].adj = new vector<int>;
        nodes[i].trueAdj = new vector<int>;
        //nodes[i].ancestors = new vector<int>;
        bits.at(i) = false;
        leaves.at(i) = true;
    }
    this->build(fp);

}

Graph::Graph(int nNodes) :
        nNodes(nNodes) {
    nodes.resize(nNodes);
    for (int i = 0; i < nNodes; i++)
        nodes[i].id = i;
}

void Graph::printGraph() {
    for (int v = 0; v < nNodes; ++v) {
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        for (auto x : *nodes[v].adj)
            cout << "-> " << x;
        printf("\n");
    }
}

void Graph::printTrueGraph() {
    for (int v = 0; v < nNodes; ++v) {
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        for (auto x : *nodes[v].trueAdj)
            cout << "-> " << x;
        printf("\n");
    }
}

void Graph::printTrueGraphSize() {
    for (int v = 0; v < nNodes; ++v) {
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        for (auto x : *nodes[v].trueAdj)
            cout << "-> " << x;
        printf("\nSub-graph size: %d\n", nodes[v].subTreeSize);
    }
}

void Graph::printTrueLabels() {
    for (int v = 0; v < nNodes; ++v) {
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        for (auto x : *nodes[v].trueAdj)
            cout << "-> " << x;
        printf("\nSub-graph size: %d", nodes[v].subTreeSize);
        printf("\nSub-graph size: %d -> %d\n", nodes[v].start, nodes[v].end);
    }
}

void Graph::sortVectors() {
    for (int v = 0; v < nNodes; ++v) {
        sort(nodes[v].adj->begin(), nodes[v].adj->end(), std::less<int>());
    }
}

void Graph::addEdge(int u, int v) {
    nodes[u].adj->push_back(v);
    bits.at(v) = true;
}

void Graph::build_addEdges(unsigned u, vector<unsigned>& adj, unsigned adj_size) {
    if (nodes[u].adj->size() == 0) {
        nodes[u].adj->resize(adj_size);
        for (int i = 0; i < adj_size; i++) {
            nodes[u].adj->at(i) = adj[i];
            bits.at(adj[i]) = true;
        }
    } else
        nodes[u].adj->insert(nodes[u].adj->end(), &adj[0], &adj[adj_size]);
}

void Graph::build(FILE * fp) {
    unsigned u, v;
    unsigned max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char dontcare[3];
    vector<unsigned> buf = vector<unsigned> (nNodes + 1);

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
            buf[i++] = v;

            token = strtok(NULL, " ");
        }

        this->build_addEdges(u, buf, i);

    }

}

std::vector<bool> Graph::returnRoots() {
    return bits;
}