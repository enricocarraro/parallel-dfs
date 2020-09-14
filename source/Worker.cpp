//
// Created by lire on 8/19/20.
//

#include "Worker.h"

/*Worker::Worker(int id) {
    //this->id = id;
}*/


using namespace std;

void Worker::initialize (int id, Graph *g, int nWorkers) {
    this->id = id;
    this->g = g;
    this->graphSize = g->nNodes/nWorkers + 1;
    neighbours.resize((graphSize));
    neighboursWeights.resize((graphSize));
}

void Worker::resetSemaphores() {

    askManagerToEmpty->reset();
    askManagerToFeed->reset();
    managerHasFed->reset();
}

void Worker::preGraphSize() {
    Node *n;
    int positionIntoGraphVector = 0;
    intCouple toPush;

    managerHasFed->wait();
    n = next;
    askManagerToFeed->signal();
    while (n->id != -1) {
        toPush.adj = vector<int> (1, n->descendantSize);
        toPush.father = n->id;
        //cout << "Working on " << n.id << "\n";
        neighbours.at(positionIntoGraphVector) = toPush;
        askManagerToEmpty->signal();
        positionIntoGraphVector = (positionIntoGraphVector+1)%graphSize;
        managerHasFed->wait();
        n = next;
        askManagerToFeed->signal();
    }
}

void Worker::work() {
    Node *n;
    int positionIntoGraphVector = 0, adjSize, weight;
    intVetVet toPush;
    managerHasFed->wait();
    n = next;
    askManagerToFeed->signal();
    while (n->id != -1) {
        toPush.adj = *n->adj;
        adjSize = toPush.adj.size();
        toPush.adjWeights = new std::vector<int> (adjSize);
        toPush.father = n->id;
        weight = 0;
        for(int i=0; i<adjSize; i++) {
            toPush.adjWeights->at(i) = 1 + weight + g->nodes.at(toPush.father).fatherWeight;
            //toPush.adjWeights->at(i) = 1 + weight + g->nodesWeights.at(toPush.father);
        }
        //cout << "Working on " << n.id << "\n";
        neighboursWeights.at(positionIntoGraphVector) = toPush;
        positionIntoGraphVector = (positionIntoGraphVector+1)%graphSize;
        askManagerToEmpty->signal();
        managerHasFed->wait();
        n = next;
        askManagerToFeed->signal();
    }
}

void Worker::subGraphSize() {
    Node *n;
    int positionIntoGraphVector = 0;
    intCouple toPush;

    managerHasFed->wait();
    n = next;
    askManagerToFeed->signal();
    while (n->id != -1) {
        toPush.adj = vector<int> (1);
        toPush.adj.at(0) = n->subTreeSize;
        toPush.father = n->id;
        //cout << "Working on " << n.id << "\n";
        neighbours.at(positionIntoGraphVector) = toPush;
        askManagerToEmpty->signal();
        positionIntoGraphVector = (positionIntoGraphVector+1)%graphSize;
        managerHasFed->wait();
        n = next;
        askManagerToFeed->signal();
    }
}

void Worker::labels() {
    Node *n;
    int positionIntoGraphVector = 0;
    intCouple toPush;


    managerHasFed->wait();
    n = next;
    askManagerToFeed->signal();

    while (n->id != -1) {
        n->end = n->start + n->subTreeSize - 1;
        toPush.adj = *n->trueAdj;
        toPush.father = n->start;
        //cout << "Working on " << n.id << "\n";
        neighbours.at(positionIntoGraphVector) = toPush;
        askManagerToEmpty->signal();
        positionIntoGraphVector = (positionIntoGraphVector+1)%graphSize;
        managerHasFed->wait();
        n = next;
        askManagerToFeed->signal();
    }
}