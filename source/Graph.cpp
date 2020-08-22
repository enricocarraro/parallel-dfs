#include "Graph.h"
#include "GraphBuilder.h"

using namespace std;

Graph::Graph(FILE *fp) {
    fscanf(fp, "%d", &nNodes);
    nodes.resize(nNodes);
    bits.resize(nNodes);
    for (int i = 0; i < nNodes; i++) {
        nodes[i].id = i;
        bits.at(i) = false;
    }
    this->build(fp);

}

/*
Graph::Graph(int nNodes) :
        nNodes(nNodes) {
    nodes.resize(nNodes);
    for (int i = 0; i < nNodes; i++)
        nodes[i].id = i;
}
 */

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

void Graph::addEdges(int u, vector<int> adj, int adj_size) {
    if (nodes[u].adj.size() == 0) {
        nodes[u].adj.resize(adj_size);
        for (int i = 0; i < adj_size; i++) {
            nodes[u].adj[i] = adj.at(i);
            bits.at(adj[i]) = true;
        }
    } else
        nodes[u].adj.insert(nodes[u].adj.end(), &adj[0], &adj[adj_size]);
}

void startBuilders (GraphBuilder *gb) {
    gb->buildLoop();
}

void Graph::build(FILE *fp) {
    unsigned max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char tmp[3];
    int whoNext = 0;
    vector<thread> tBuilders(GraphBuilderN);

    vector<GraphBuilder> gb(GraphBuilderN);
    for(int i = 0; i<GraphBuilderN; i++) {
        gb.at(i).initialize(*this, nNodes);
        tBuilders[i] = thread(startBuilders, &gb.at(i));
    }

    while (true) {
        if(fscanf(fp, "%[^#]s", str) != EOF) {
            gb.at(whoNext).free.wait();
            gb.at(whoNext).str = string(str);
            gb.at(whoNext).working.signal();
            whoNext = (whoNext+1)%GraphBuilderN;
        }
        else {
            for(int i = 0; i<GraphBuilderN; i++) {
                gb.at((whoNext+i)%GraphBuilderN).free.wait();
                gb.at((whoNext+i)%GraphBuilderN).str = string("end");
                gb.at((whoNext+i)%GraphBuilderN).working.signal();
            }
            break;
        }
        fscanf(fp, "%s", tmp);

    }


    for(int i = 0; i<GraphBuilderN; i++) {
        tBuilders[i].join();
    }

}

std::vector<bool> Graph::returnRoots() {
    return bits;
}
