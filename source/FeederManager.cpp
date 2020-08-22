//
// Created by lire on 8/19/20.
//

#include "FeederManager.h"

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
}

void feederManager::feedLoop() {
    Node *toPush;
    Node terminator;
    terminator.id = -1;
    int currentWorker = 0;
    intint next;
    while (true) // starting nodes (with no parent)
    {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize*queueSize);
        if (next.son == -1) {
            break;
        }
        toPush = &g->nodes.at(next.son);
        toPush->father = next.father;
        workers->at(currentWorker).askManagerToFeed->wait();
        workers->at(currentWorker).next = *toPush;
        workers->at(currentWorker).managerHasFed->signal();
        currentWorker = (currentWorker + 1) % nWorkers;
        nodeRead++;
    }
    while (nodeRead < graphSize) //manca condizione di stop
    {
        commonSemQueueFull->wait();
        next = commonQueue->at(queueExtractPosition);
        commonSemQueueEmpty->signal();
        queueExtractPosition = (queueExtractPosition + 1) % (graphSize*queueSize);
        if (next.son == -1) {
            continue;
        }
        toPush = &g->nodes.at(next.son);
        toPush->father = next.father;
        workers->at(currentWorker).askManagerToFeed->wait();
        workers->at(currentWorker).next = *toPush;
        workers->at(currentWorker).managerHasFed->signal();
        currentWorker = (currentWorker + 1) % nWorkers;
        g->nodes.at(next.father).trueAdj.push_back(next.son);
        nodeRead++;
        //condizione di stop
    }
    for (int i = 0; i < nWorkers; i++) {
        workers->at((currentWorker + i) % nWorkers).askManagerToFeed->wait();
        workers->at((currentWorker + i) % nWorkers).next = terminator;
        workers->at((currentWorker + i) % nWorkers).managerHasFed->signal();
    }
}