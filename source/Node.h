//
// Created by lire on 10/25/20.
//

#ifndef SDP_PARALLELSOLUTION_NODE_H
#define SDP_PARALLELSOLUTION_NODE_H

#include <vector>
#include <atomic>
#if USE_BSEM
#include "BusySemaphore.h"
#endif

struct Node {

    int id;

    int start = INT32_MAX;
    int end = -1;


    std::vector<int> adj;
    int adjSize = 0;

    bool root = true;

    Node() {
        nextNode.resize(N_THREADS+1, -1);
        nextNodeToVisit.resize(N_THREADS+1, -1);
        prevNode.resize(N_THREADS+1, -1);
        prevNodeToVisit.resize(N_THREADS+1, -1);
        visitato.resize(N_THREADS+1, false);
        //visitatoInQuestoLivello.resize(N_THREADS+1, -1);
        pronto.resize(N_THREADS+1, -1);
        valoreCountdown.resize(N_THREADS+1, N_THREADS);

#if USE_BSEM
        for(int i=0; i<N_THREADS+1; i++){
            bs.push_back(new BusySemaphore(true));
        }
#endif
    }
#if USE_BSEM
    ~Node() { for (BusySemaphore *b : bs) delete b; delete nVisitedFathers; }
#else

    ~Node() { delete nVisitedFathers; delete ultimaEpoca; delete ultimaEpocaFantasma; }
#endif

    std::vector<int> nextNode;
    std::vector<int> nextNodeToVisit;
    std::vector<int> prevNode;
    std::vector<int> prevNodeToVisit;
    std::vector<int> pronto;
    std::vector<char> visitato; //potrebbe essere una variabile invece che un vettore
    //std::vector<int> visitatoInQuestoLivello;
    std::vector<int> valoreCountdown;

#if USE_BSEM
    std::vector<BusySemaphore *> bs;
#endif

    std::atomic<int> *ultimaEpoca = new std::atomic<int> (-1);
    std::atomic<int> *ultimaEpocaFantasma = new std::atomic<int> (-1);
    std::atomic<int> *nVisitedFathers = new std::atomic<int> (0);
    int nFathers = 0;
    std::atomic<int> *countdown = new std::atomic<int> (N_THREADS);
};

#endif //SDP_PARALLELSOLUTION_NODE_H
