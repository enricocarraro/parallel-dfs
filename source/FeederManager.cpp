//
// Created by lire on 8/19/20.
//

#include "FeederManager.h"

using namespace std;

feederManager::feederManager(vector<Worker> *allWorkers, int nWorkers,
                             Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                             std::vector<intintint> *commonQueue, Graph *g) {
    this->workers = allWorkers;
    this->commonSemQueueFull = commonSemQueueFull;
    this->commonSemQueueEmpty = commonSemQueueEmpty;
    this->nWorkers = nWorkers;
    //this->separator = separator;
    this->commonQueue = commonQueue;
    this->graphSize = g->nNodes;
    this->g = g;
    terminator.id = -1;
}

void feederManager::preGraphSize()
{
    int queueExtractPosition = 0;
    int nodeRead = 0;
    vector<int> visitedChilds(graphSize, 0);
    intintint next;
    int currentWorker = 0;
    int nextFather;

    while (nodeRead < graphSize) {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);

        int ancSize = g->nodes.at(next.father).ancSize;
        for(int i=0; i<ancSize; i++) {
            nextFather = g->nodes.at(next.father).ancestors->at(i);
            if (nextFather >= 0) {
                visitedChilds.at(nextFather)++;
                unsigned long int test = g->nodes.at(nextFather).descendantSize;
                g->nodes.at(nextFather).descendantSize += (next.weight);
                if(test>g->nodes.at(nextFather).descendantSize) {
                    printf("Carry");
                }
                if (g->nodes.at(nextFather).adjSize == visitedChilds.at(nextFather)) {
                    workers->at(currentWorker).askManagerToFeed->wait();
                    workers->at(currentWorker).next = &g->nodes.at(nextFather);
                    workers->at(currentWorker).managerHasFed->signal();
                    currentWorker = (currentWorker + 1) % nWorkers;
                }
            }
        }
        if (ancSize == 0) {
            g->preorderVetSize += next.weight;
        }
        nodeRead++;
    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = &terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
}

void feederManager::feedLoop() {
    int queueExtractPosition = 0;
    int nodeRead = 0;
    Node *toPush;
    int currentWorker = 0;
    intintint next;
    intint cancelled;
    vector<int> visitedFathers (graphSize, 0);

    while (true) // starting nodes (with no parent)
    {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);

        if (next.child == -1) {
            //g->nodes.at(next.father).exitingArcs = g->nodes.at(next.father).trueAdj.size();
            break;
        }
        toPush = &g->nodes.at(next.child);
        //toPush->father = next.father;
        toPush->fatherWeight = next.weight;
        //g->nodesWeights.at(next.child) = next.weight;
        workers->at(currentWorker).askManagerToFeed->wait();
        workers->at(currentWorker).next = toPush;
        workers->at(currentWorker).managerHasFed->signal();
        currentWorker = (currentWorker + 1) % nWorkers;
        nodeRead++;

        //fill preorder vector
        //g->preorder.at(next.weight) = next.child;
    }
    while (nodeRead < graphSize) //nodes with parents
    {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);

        /*
        if (next.child == -1) {
            //g->nodes.at(next.father).exitingArcs = g->nodes.at(next.father).trueAdj->size();
            continue;
        }
         */
        toPush = &g->nodes.at(next.child);
        //if (g->nodesWeights.at(next.child) > next.weight) {
            //g->nodesWeights.at(next.child) = next.weight;

        if (toPush->fatherWeight <= next.weight) {
            cancelled.father = next.father;
            cancelled.child = next.child;
            g->cancelledEdges->at(g->posIntoCancelledEdges++) = cancelled;
        }
        else {
            if(toPush->father != -1) {
                cancelled.father = toPush->father;
                cancelled.child = next.child;
                g->cancelledEdges->at(g->posIntoCancelledEdges++) = cancelled;
            }
            toPush->father = next.father;   //set the father of the node
            toPush->fatherWeight = next.weight;
        }
        visitedFathers.at(next.child)++;
        if(visitedFathers.at(next.child) == toPush->ancSize) {
            g->leaves.at(toPush->father) = false;  //fathers with one or more sons are not leaves
            workers->at(currentWorker).askManagerToFeed->wait();
            workers->at(currentWorker).next = toPush;
            workers->at(currentWorker).managerHasFed->signal();
            currentWorker = (currentWorker + 1) % nWorkers;
            g->nodes.at(toPush->father).trueAdj->push_back(
                    next.child);   //might be replaced with a sized vector and then resized
            nodeRead++;
        }

    }
    //g->nodes.at(next.father).exitingArcs = g->nodes.at(next.father).trueAdj->size();
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = &terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
    for(int i=0; i<graphSize; i++) {
        g->nodes.at(i).exitingArcs = g->nodes.at(i).trueAdj->size();
        if(visitedFathers.at(i) != g->nodes.at(i).ancSize) {
            for(auto y : *g->nodes.at(i).adj) {
                cancelled.father = i;
                cancelled.child = y;
                g->cancelledEdges->at(g->posIntoCancelledEdges++) = cancelled;
            }
        }
    }
}

void feederManager::subGraphSize()
{
    int queueExtractPosition = 0;
    int nodeRead = 0;
    vector<int> visitedChilds(graphSize, 0);
    intintint next;
    int currentWorker = 0;
    int nextFather;

    while (nodeRead < graphSize) {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);

        nextFather = g->nodes.at(next.father).father;
        if (nextFather >= 0) {
            visitedChilds.at(nextFather)++;
            g->nodes.at(nextFather).subTreeSize += (next.child);
            if (g->nodes.at(nextFather).exitingArcs == visitedChilds.at(nextFather)) {
                workers->at(currentWorker).askManagerToFeed->wait();
                workers->at(currentWorker).next = &g->nodes.at(nextFather);
                workers->at(currentWorker).managerHasFed->signal();
                currentWorker = (currentWorker + 1) % nWorkers;
            }
        }
        nodeRead++;
    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = &terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
}

void feederManager::labels() {
    int queueExtractPosition = 0;
    int nodeRead = 0;
    intintint next;
    Node *toPush;
    int currentWorker = 0;

    while (nodeRead < graphSize) {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);

        toPush = &g->nodes.at(next.father);
        toPush->start = next.child;
        g->st_father->at(toPush->id).fathers = new vector<int> (1, toPush->father);
        g->st_father->at(toPush->id).start = toPush->start;


        workers->at(currentWorker).askManagerToFeed->wait();
        workers->at(currentWorker).next = toPush;
        workers->at(currentWorker).managerHasFed->signal();
        currentWorker = (currentWorker + 1) % nWorkers;

        nodeRead++;
    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = &terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
}