//
// Created by lire on 8/19/20.
//

#include "Worker.h"

/*Worker::Worker(int id) {
    //this->id = id;
}*/

void Worker::work() {
    Node n;
    //int father;
    intCouple toPush;
    managerHasFed->wait();
    //cout << "Worker " << id << " started working\n";
    n = next;
    askManagerToFeed->signal();
    while (n.id != -1) {
        toPush.adj = n.adj;
        toPush.father = n.id;
        //cout << "Working on " << n.id << "\n";
        queueExclusion->wait();
        neighbours.push(toPush);
        queueExclusion->signal();
        askManagerToEmpty->signal();
        managerHasFed->wait();
        n = next;
        askManagerToFeed->signal();
    }
}
