//
// Created by lire on 8/19/20.
//

#include "Graph.h"

using namespace std;

Graph::Graph(FILE *fp) {

    fscanf(fp, "%d\n", &nNodes);
    nodes.resize(nNodes+2);

    rootsSize = nNodes;

#if READ_TYPE == 0
    this->build(fp);
#elif READ_TYPE == 1
    this->build2(fp);
#elif READ_TYPE == 2
    this->build3(fp);
#else
    printf("Read option not supported");
    exit(-1);
#endif

    roots.resize(rootsSize);
    nodes.at(nNodes).root = false;
    nodes.at(nNodes+1).root = false;

    for (int i = 0; i < nNodes+2; i++) {

        if (nodes[i].root) {
            roots.at(rootsPos++) = i;
        }
        nodes[i].id = i;

    }

    startingNode = nNodes;
    endingNode = nNodes+1;
    startNode = &nodes.at(startingNode);
    endNode = &nodes.at(endingNode);
    //startNode->pronto.at(0) = true;
    for(int i = 0; i < N_THREADS+1; i++) {
        startNode->nextNode.at(i) = endingNode;
        startNode->nextNodeToVisit.at(i) = endingNode;
        startNode->visitato.at(i) = true;
        endNode->prevNode.at(i) = startingNode;
        endNode->prevNodeToVisit.at(i) = startingNode;
        endNode->nextNode.at(i) = endingNode;
        endNode->nextNodeToVisit.at(i) = endingNode;
        endNode->visitato.at(i) = true;
        //endNode->pronto.at(i) = true;
        //endNode->bs.at(i)->signal();
    }
}

void Graph::sortVectors() {
    for (int v = 0; v < nNodes; ++v) {
        sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less<int>());
    }
}

void Graph::build_addEdges(unsigned u, vector<unsigned> &adj, unsigned adj_size) {
    if (adj_size > 0) {
        nodes[u].adj.resize(adj_size);
        for (int i = 0; i < adj_size; i++) {
            nodes[u].adj.at(i) = adj[i];
            if (nodes.at(adj[i]).root) {
                nodes.at(adj[i]).root = false;
                rootsSize--;
            }
            nodes.at(adj[i]).nFathers++;
        }
        nodes[u].adjSize = nodes[u].adj.size();
    }
}

#if READ_TYPE == 0
void Graph::build(FILE *fp) {
    unsigned u;
    unsigned max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char dontcare[3];
    vector<unsigned> buf = vector<unsigned>(nNodes + 1);

    while (fscanf(fp, "%[^#]s", str) != EOF) {
        fscanf(fp, "%s\n", dontcare);
        unsigned i = 0, j = 0;
        u = 0;
        while(str[i] != ':') {
            u = u * 10 + (str[i++] & 0x0f);
        }
        i = i+2;
        while (str[i] != '\0') {
            buf.at(j) = 0;
            while(str[i] != ' ') {
                buf.at(j) = buf.at(j) * 10 + (str[i++] & 0x0f);
            }
            j++; i++;
        }
        build_addEdges(u, buf, j);
    }
}
#elif READ_TYPE == 1
void Graph::build2(FILE *fp) {
    unsigned u;
    char str;
    vector<unsigned> buf;

    while (fscanf(fp, "%c", &str) != EOF) {
        u = 0;
        while(str != ':') {
            u = u * 10 + (str & 0x0f);
            fscanf(fp, "%c", &str);
        }
        fscanf(fp, "%c", &str);
        fscanf(fp, "%c", &str);

        while(str != '\n') {
            while(str != '#') {
                buf.push_back(0);
                while(str != ' ') {
                    buf.back() = buf.back() * 10 + (str & 0x0f);
                    fscanf(fp, "%c", &str);
                }
                fscanf(fp, "%c", &str);
            }
            build_addEdges(u, buf, buf.size());
            fscanf(fp, "%c", &str);
        }
        buf.clear();
    }
}
#elif READ_TYPE == 2
void Graph::build3(FILE *fp) {
    unsigned u;
    char str;
    vector<unsigned> buf;
    unsigned val;

    while (fscanf(fp, "%d: ", &u) != EOF) {
        while(fscanf(fp, "%d ", &val)) {
            buf.push_back(val);
        }
        build_addEdges(u, buf, buf.size());
        buf.clear();
        fscanf(fp, "#\n");
    }
}
#endif