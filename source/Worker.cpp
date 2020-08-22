//
// Created by lire on 8/19/20.
//

#include "Worker.h"

/*Worker::Worker(int id, int graphSize) {
    this->id = id;
    this->graphSize = graphSize;
    askManagerToEmpty.resize(0, graphSize);
    neighbours.resize(graphSize);
    //managerHasEmptied.resize(graphSize, graphSize);
}*/

void Worker::initialize (int id, int graphSize, int nWorkers) {
    this->id = id;
    this->graphSize = graphSize/nWorkers;
    neighbours.resize((graphSize/nWorkers)+1);
}

void Worker::work() {
    Node n;
    //int father;
    intCouple toPush;
    managerHasFed.wait();
    //cout << "Worker " << id << " started working\n";
    n = next;
    askManagerToFeed.signal();
    while (n.id != -1) {
        toPush.adj = n.adj;
        toPush.father = n.id;
        //cout << "Working on " << n.id << "\n";
        //queueExclusion.wait();
        neighbours.at(positionIntoGraphVector) = toPush;
        //queueExclusion.signal();
        positionIntoGraphVector = (positionIntoGraphVector+1)%graphSize;
        askManagerToEmpty.signal();
        managerHasFed.wait();
        n = next;
        askManagerToFeed.signal();
    }
}
