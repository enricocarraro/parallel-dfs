//
// Created by lire on 8/19/20.
//

#include "EmptierManager.h"

using namespace boost::multiprecision;
using namespace std;

emptierManager::emptierManager(vector<Worker> *allWorkers, int nWorkers,
                               Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                               std::vector<intintint> *commonQueue, Graph *g) {
    this->workers = allWorkers;
    this->commonSemQueueFull = commonSemQueueFull;
    this->commonSemQueueEmpty = commonSemQueueEmpty;
    this->nWorkers = nWorkers;
    this->commonQueue = commonQueue;
    this->graphSize = g->nNodes;
    this->g = g;
    roots = &g->roots;
    preLeaves = &g->preLeaves;
}

void emptierManager::preGraphSize() {
    int queueInsertPosition = 0;
    int nodeRead = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);

    intintint toPush;
    toPush.prefix = 1;       // # of descendants

    int i;

    for (i = 0; i < preLeaves->size(); i++) {
        toPush.start = preLeaves->at(i);      // the node itself
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPush;
        commonSemQueueFull->signal();
        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
        nodeRead++;
    }

    i = 0;
    while (nodeRead < graphSize) {
        workers->at(i).askManagerToEmpty->wait();
        toPush.start = workers->at(i).neighboursWeights.at(positionsIntoWorkQueues[i]).father; //node itself
        toPush.prefix = workers->at(i).neighboursWeights.at(positionsIntoWorkQueues[i]).adjWeights->at(0); //weight
        positionsIntoWorkQueues[i] = (positionsIntoWorkQueues[i] + 1) % workers->at(i).graphSize;

        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPush;
        commonSemQueueFull->signal();
        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

        nodeRead++;
        i = (i + 1) % nWorkers;
    }
}


void emptierManager::weightsAndPrefixes() {
    int queueInsertPosition = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    intintint toPushWeight;
    int readNodes = 0;

    uint1024_t prefix = 0;
    std::vector<int> *childs;
    boostIntVect *next;

    int i, j;

    for (int i = 0; i < roots->size(); i++) {    //"node" containing all other root nodes as neighbours
        toPushWeight.child = roots->at(i);
        toPushWeight.prefix = prefix;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPushWeight;
        commonSemQueueFull->signal();

        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

        prefix += g->nodes.at(toPushWeight.child).nodeWeight;
    }
    readNodes++;

    i = 0;
    while (readNodes < graphSize) {

        workers->at(i).askManagerToEmpty->wait();

        next = &workers->at(i).nextWeights.at(positionsIntoWorkQueues[i]++);
        childs = next->childs;
        prefix = next->prefix;
        for (j = 0; j < childs->size(); j++) {    //"node" containing all other root nodes as neighbours
            toPushWeight.child = childs->at(j);
            toPushWeight.prefix = prefix;
            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPushWeight;
            commonSemQueueFull->signal();

            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

            prefix += g->nodes.at(childs->at(j)).nodeWeight;
        }
        readNodes++;

        i = (i + 1) % nWorkers;
    }
}


void emptierManager::startEndTimes() {
    int queueInsertPosition = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    intintint toPushWeight;
    int readNodes = 0;

    std::vector<int> *fathers;
    intCouple *next;

    int i, j;

    for (i = 0; i < preLeaves->size(); i++) {    //"node" containing all other root nodes as neighbours
        toPushWeight.child = preLeaves->at(i);
        toPushWeight.start = g->nodes.at(toPushWeight.child).end;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPushWeight;
        commonSemQueueFull->signal();

        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
    }
    readNodes++;


    i = 0;
    while (readNodes < graphSize) {

        workers->at(i).askManagerToEmpty->wait();

        next = &workers->at(i).nextStart.at(positionsIntoWorkQueues[i]++);
        toPushWeight.start = next->start;
        fathers = next->fathers;
        for (j = 0; j < fathers->size(); j++) {    //"node" containing all other root nodes as neighbours
            toPushWeight.child = fathers->at(j);
            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPushWeight;
            commonSemQueueFull->signal();

            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
        }
        readNodes++;

        i = (i + 1) % nWorkers;
    }
}
