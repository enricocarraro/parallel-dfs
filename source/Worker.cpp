//
// Created by lire on 8/19/20.
//

#include "Worker.h"

/*Worker::Worker(int id) {
    //this->id = id;
}*/

void Worker::initialize (int id, int graphSize, int nWorkers) {
    this->id = id;
    this->graphSize = graphSize/nWorkers;
    neighbours.resize((graphSize/nWorkers)+1);
}

void Worker::resetSemaphores() {

    askManagerToEmpty->reset();
    askManagerToFeed->reset();
    managerHasFed->reset();
}

void Worker::work() {
    Node *n;
    int positionIntoGraphVector = 0;
    intCouple toPush;
    managerHasFed->wait();
    n = next;
    askManagerToFeed->signal();
    while (n->id != -1) {
        toPush.adj = n->adj;
        toPush.father = n->id;
        //cout << "Working on " << n.id << "\n";
        neighbours.at(positionIntoGraphVector) = toPush;
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
        toPush.adj = n->trueAdj;
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