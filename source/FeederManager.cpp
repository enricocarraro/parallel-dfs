//
// Created by lire on 8/19/20.
//

#include "FeederManager.h"

using namespace std;

feederManager::feederManager(vector<Worker> *allWorkers, int nWorkers,
                             Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                             std::vector<intint> *commonQueue, Graph *g) {
    this->workers = allWorkers;
    this->commonSemQueueFull = commonSemQueueFull;
    this->commonSemQueueEmpty = commonSemQueueEmpty;
    this->nWorkers = nWorkers;
    //this->separator = separator;
    this->commonQueue = commonQueue;
    this->graphSize = g->size();
    this->g = g;
    terminator.id = -1;
}

void feederManager::feedLoop() {
    int queueExtractPosition = 0;
    int nodeRead = 0;
    Node *toPush;
    int currentWorker = 0;
    intint next;
    vector<bool> visitedFathers = vector<bool> (graphSize, false);

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
        toPush->father = next.father;
        workers->at(currentWorker).askManagerToFeed->wait();
        workers->at(currentWorker).next = toPush;
        workers->at(currentWorker).managerHasFed->signal();
        currentWorker = (currentWorker + 1) % nWorkers;
        nodeRead++;
    }
    while (nodeRead < graphSize) //nodes with parents
    {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize);
        if (next.child == -1) {
            g->nodes.at(next.father).exitingArcs = g->nodes.at(next.father).trueAdj->size();
            continue;
        }
        toPush = &g->nodes.at(next.child);
        toPush->father = next.father;   //set the father of the node
        g->leaves.at(next.father) = false;  //fathers with one or more sons are not leaves
        visitedFathers.at(next.father) = true;
        workers->at(currentWorker).askManagerToFeed->wait();
        workers->at(currentWorker).next = toPush;
        workers->at(currentWorker).managerHasFed->signal();
        currentWorker = (currentWorker + 1) % nWorkers;
        g->nodes.at(next.father).trueAdj->push_back(next.child);   //might be replaced with a sized vector and then resized
        nodeRead++;

    }
    g->nodes.at(next.father).exitingArcs = g->nodes.at(next.father).trueAdj->size();
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = &terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
    for(int i=0; i<graphSize; i++) {
        if(!visitedFathers.at(i)) {
            for(auto y : *g->nodes.at(i).adj) {
                intint cancelled;
                cancelled.father = i;
                cancelled.child = y;
                g->cancelledEdges->push_back(cancelled);
            }
        }
    }
}

void feederManager::subGraphSize()
{
    int queueExtractPosition = 0;
    int nodeRead = 0;
    vector<int> visitedChilds(graphSize, 0);
    intint next;
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
    intint next;
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