//
// Created by lire on 8/19/20.
//

#include "Worker.h"



using namespace boost::multiprecision;
using namespace std;

void Worker::initialize(int id, Graph *g, int nWorkers) {
    this->id = id;
    this->g = g;
    this->graphSize = g->nNodes / nWorkers + 1;
#if !LIMITED_SPACE
    nextStart.resize(graphSize);
    neighboursWeights.resize(graphSize);
    nextWeights.resize(graphSize);
#else
    results.resize(graphSize);
#if USE_QUICK_SEM
    managerHasEmptied = new FastSemaphore (graphSize);
#else
    managerHasEmptied = new Semaphore (graphSize, graphSize);
#endif
#endif
}

void Worker::resetSemaphores() {
    askManagerToEmpty->reset();
    askManagerToFeed->reset();
    managerHasFed->reset();
    managerHasEmptied->reset();
}

void Worker::preGraphSize() {
    Node *n;
    int positionIntoGraphVector = 0;
    boostIntVectBoostVect toPush;

    managerHasFed->wait();
    n = next;
    askManagerToFeed->signal();
    while (n->id != -1) {
        toPush.adjWeights = new vector<uint1024_t>(1, n->nodeWeight);
        toPush.father = n->id;
        managerHasEmptied->wait();
        results.at(positionIntoGraphVector) = toPush;
        askManagerToEmpty->signal();

        positionIntoGraphVector = (positionIntoGraphVector + 1) % graphSize;

        managerHasFed->wait();
        n = next;
        askManagerToFeed->signal();
    }
}

void Worker::weightsAndPrefixes() {
    Node *n;
    int positionIntoGraphVector = 0;
    uint1024_t weight;
    boostIntVectBoostVect toPush;

    managerHasFed->wait();
    n = next;
    askManagerToFeed->signal();

    while (n->id != -1) {

        n->nodeWeight += n->prefix;

        toPush.adj = n->adj;
        toPush.father = n->prefix;

        managerHasEmptied->wait();
        results.at(positionIntoGraphVector) = toPush;
        askManagerToEmpty->signal();

        positionIntoGraphVector = (positionIntoGraphVector + 1) % graphSize;

        managerHasFed->wait();
        n = next;
        askManagerToFeed->signal();
    }
}

void Worker::startEndTimes() {
    Node *n;
    int positionIntoGraphVector = 0;
    uint1024_t weight;
    boostIntVectBoostVect toPush;

    managerHasFed->wait();
    n = next;
    askManagerToFeed->signal();

    while (n->id != -1) {

        toPush.adj = n->ancestors;
        toPush.father = n->start;

        managerHasEmptied->wait();
        results.at(positionIntoGraphVector) = toPush;
        askManagerToEmpty->signal();

        positionIntoGraphVector = (positionIntoGraphVector + 1) % graphSize;

        managerHasFed->wait();
        n = next;
        askManagerToFeed->signal();
    }
}