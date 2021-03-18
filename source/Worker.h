//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_WORKER_H
#define SDP_PIPELINERESOLUTION_WORKER_H

#include <vector>

#include "Graph.h"
#include "OPTIONS.h"
#include "BusySemaphore.h"


class Worker {

    bool checkSuccessivo(Node *n, int epoca, int epocaAttuale);
    bool controllaValidita(Node *n, int epoca, int epocaAttuale);
    void insertGrigio(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted);
    void insertGiallo(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted);
    bool insertArancio(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted);

public:

    BusySemaphore *sem;
    Graph *g;

    int start(int position);
    int start2(int position);

    void initialize(Graph *g);

};

#endif //SDP_PIPELINERESOLUTION_WORKER_H
