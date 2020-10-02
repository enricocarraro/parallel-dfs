//
// Created by lire on 8/19/20.
//

#include "FeederManager.h"

using namespace std;

#if !USE_QUICK_SEM
feederManager::feederManager(vector<Worker> *allWorkers, int nWorkers,
                             Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                             std::vector<intintint> *commonQueue, Graph *g) {
#else
feederManager::feederManager(vector<Worker> *allWorkers, int nWorkers,
                             FastSemaphore *commonSemQueueFull, FastSemaphore *commonSemQueueEmpty,
                             std::vector<intintint> *commonQueue, Graph *g) {
#endif
    this->workers = allWorkers;
    this->commonSemQueueFull = commonSemQueueFull;
    this->commonSemQueueEmpty = commonSemQueueEmpty;
    this->nWorkers = nWorkers;
    this->commonQueue = commonQueue;
    this->graphSize = g->nNodes;
    this->g = g;
    terminator.id = -1;
}

void feederManager::preGraphSize() {
    int queueExtractPosition = 0;
    int nodeRead = 0;
    vector<int> visitedChilds(graphSize, 0);
    intintint next;
    int currentWorker = 0;
    int nextFather;
    int ancSize;

    while (nodeRead < graphSize) {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);

        ancSize = g->nodes.at(next.start).ancSize;
        for (int i = 0; i < ancSize; i++) {
            nextFather = g->nodes.at(next.start).ancestors->at(i);
            if (nextFather >= 0) {
                visitedChilds.at(nextFather)++;
                g->nodes.at(nextFather).nodeWeight += (next.prefix);
                if (g->nodes.at(nextFather).adjSize == visitedChilds.at(nextFather)) {
                    workers->at(currentWorker).askManagerToFeed->wait();
                    workers->at(currentWorker).next = &g->nodes.at(nextFather);
                    workers->at(currentWorker).managerHasFed->signal();
                    currentWorker = (currentWorker + 1) % nWorkers;
                }
            }
        }
        if (ancSize == 0) {
            g->preorderVetSize += next.prefix;
        }
        nodeRead++;
    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = &terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
    }
}

void feederManager::weightsAndPrefixes() {
    int queueExtractPosition = 0;
    int nodeRead = 0;
    Node *toPush;
    int currentWorker = 0;
    intintint next;
    vector<int> visitedFathers(graphSize, 0);

    while (nodeRead < graphSize) //nodes with parents
    {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);

        toPush = &g->nodes.at(next.child);

        if (toPush->prefix > next.prefix) {
            toPush->prefix = next.prefix;    //this vector contains the quantity to add to descendantSize
        }

        visitedFathers.at(next.child)++;
        if (visitedFathers.at(next.child) >= toPush->ancSize) {

            workers->at(currentWorker).askManagerToFeed->wait();
            workers->at(currentWorker).next = toPush;
            workers->at(currentWorker).managerHasFed->signal();
            currentWorker = (currentWorker + 1) % nWorkers;
            nodeRead++;
        }

    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = &terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
    }
}

void feederManager::startEndTimes() {
    int queueExtractPosition = 0;
    int nodeRead = 0;
    Node *toPush;
    int currentWorker = 0;
    intintint next;
    vector<int> visitedChilds(graphSize, 0);

    while (nodeRead < graphSize) //nodes with parents
    {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);

        toPush = &g->nodes.at(next.child);

        if (toPush->start > next.start) {
            toPush->start = next.start;    //this vector contains the quantity to add to descendantSize
        }

        visitedChilds.at(next.child)++;
        if (visitedChilds.at(next.child) >= toPush->adjSize) {

            workers->at(currentWorker).askManagerToFeed->wait();
            workers->at(currentWorker).next = toPush;
            workers->at(currentWorker).managerHasFed->signal();
            currentWorker = (currentWorker + 1) % nWorkers;
            nodeRead++;
        }

    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = &terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
    }
}