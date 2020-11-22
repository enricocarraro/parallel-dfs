//
// Created by lire on 8/19/20.
//

#include "Graph.h"

using namespace std;

Graph::Graph(FILE *fp) {

    fscanf(fp, "%d\n", &nNodes);
    nodes.resize(nNodes+1);
    //nodesWeights.resize(nNodes, INT32_MAX);

    //leaves.resize(nNodes, true); //keeping this for now
    preLeaves.resize(nNodes);
    rootsSize = nNodes;

    for (int i = 0; i < nNodes; i++) {
        //nodes[i].id = i;
        nodes[i].adj = new vector<int>;
        //nodes[i].trueAdj = new vector<int>;
    }
    this->build(fp);
    preLeaves.resize(preLeavesPos);
    roots.resize(rootsSize);

    //30-40% faster than GRAPH_PUSHBACK
    for (int i = 0; i < nNodes; i++) {
        nodes[i].ancestors = new vector<int>(nodes[i].ancSize);

        if (nodes[i].root) { //MUST change into if(nodes[i].root)
            roots.at(rootsPos++) = i;
        }

    }
    this->reBuild(fp);


    //cancelledEdges = new vector<intint> (nEdges);
    //var = 128;
    //var += nNodes;

}

Graph::Graph(int nNodes) :
        nNodes(nNodes) {
    nodes.resize(nNodes+1);
    //for (int i = 0; i < nNodes; i++)
        //nodes[i].id = i;
}

void Graph::printTrueLabelsPreWeights() {
    for (int v = 0; v < nNodes; ++v) {
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        //for (auto x : *nodes[v].trueAdj)
        //    cout << "-> " << x;
        cout << "\nPre-weight val: " << nodes[v].nodeWeight;
        //printf("\nSub-graph size: %d", nodes[v].subTreeSize);
        printf("\nStart-end time: %d -> %d\n", nodes[v].start, nodes[v].end);
    }
}

void Graph::sortVectors() {
    for (int v = 0; v < nNodes; ++v) {
        sort(nodes[v].adj->begin(), nodes[v].adj->end(), std::less<int>());
    }
}

void Graph::build_addEdges(unsigned u, vector<unsigned> &adj, unsigned adj_size) {
    nEdges += adj_size;
    if (adj_size > 0) {
        nodes[u].adj->resize(adj_size);
        for (int i = 0; i < adj_size; i++) {
            nodes[u].adj->at(i) = adj[i];
            if (nodes.at(adj[i]).root) {
                nodes.at(adj[i]).root = false;
                rootsSize--;
            }
            nodes[adj[i]].ancSize++;
        }
        nodes[u].adjSize = nodes[u].adj->size();
    }
    else {
        preLeaves.at(preLeavesPos++) = u;
    }
}

#if GRAPH_DOUBLE_READ
void Graph::addAncestor(unsigned int u, std::vector<unsigned int> &adj, unsigned int adj_size) {
    for (int i = 0; i < adj_size; i++) {
        nodes[adj[i]].ancestors->at(nodes[adj[i]].ancNumber++) = u;
    }
}

void Graph::reBuild(FILE * fp) {
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

            sscanf(token, "%d", &v);
            buf[i++] = v;

            token = strtok(NULL, " ");
        }

        this->addAncestor(u, buf, i);

    }

}
#endif

#if GRAPH_REREAD_GRAPH

void Graph::reBuild(FILE *fp) {

    for (int i = 0; i < nNodes; i++) {
        for (int j = 0; j < nodes[i].adjSize; j++) {
            nodes[nodes[i].adj->at(j)].ancestors->at(nodes[nodes[i].adj->at(j)].ancNumber++) = i;
        }
    }

}

#endif

void Graph::build(FILE *fp) {
    unsigned u, v;
    unsigned max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char dontcare[3];
    vector<unsigned> buf = vector<unsigned>(nNodes + 1);

    while (fscanf(fp, "%[^#]s", str) != EOF) {
        fscanf(fp, "%s\n", dontcare);
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
        while (token != NULL) {
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

/*
std::vector<bool> Graph::returnRoots() {
    return roots;
}
 */