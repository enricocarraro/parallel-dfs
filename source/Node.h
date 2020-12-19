//
// Created by lire on 10/25/20.
//

#ifndef SDP_PARALLELSOLUTION_NODE_H
#define SDP_PARALLELSOLUTION_NODE_H

//#include <boost/multiprecision/cpp_int.hpp>
//#include "BusySemaphore.h"
#include <vector>

struct Node {

    int start = INT32_MAX;
    int end = -1;


    std::vector<int> adj;
    int adjSize = 0;

#if GRAPH_DOUBLE_READ | GRAPH_REREAD_GRAPH
    int ancNumber = 0;
#endif
#if !USE_BOOL
    bool root = true;
#endif
    //BusySemaphore *bSem = new BusySemaphore;

    Node() {
        nextNode.resize(N_THREADS+1, -1);
        visitato.resize(N_THREADS+1, false);
        visitatoInQuestoLivello.resize(N_THREADS+1, -1);
        pronto.resize(N_THREADS+1, false);
    }
    //~Node() { delete bSem;};

    std::vector<int> nextNode;
    std::vector<char> pronto;
    std::vector<char> visitato;
    std::vector<int> visitatoInQuestoLivello;
};

#endif //SDP_PARALLELSOLUTION_NODE_H
