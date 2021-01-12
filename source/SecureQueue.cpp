//
// Created by lire on 10/3/20.
//

#include "SecureQueue.h"

SecureQueue::SecureQueue(int queueSize, int nWorker) { //maybe nThread is not usefull
    this->queueSize = queueSize + nWorker;
    this->queue.resize(this->queueSize);
    this->control.resize(this->queueSize, false);
    this->controlN.resize(this->queueSize, 0);
    insertPosition = 0;
    extractPosition = 0;
}

int SecureQueue::push(int next) {
    int pos = insertPosition++;
    queue.at(pos) = next;

    control.at(pos) = (control.at(pos) != true);

    dataReady.signal();
    return pos;
}

int SecureQueue::pop() {
    dataReady.wait();
    int pos = extractPosition++;
    while (control.at(pos) != ready) {}

    return queue.at(pos);
}

void SecureQueue::reset() {
    for(int i=insertPosition; i<queueSize; i++) {
        control.at(i) = (control.at(i) != true);
    }
    ready = ready!=true;
    insertPosition = 0;
    extractPosition = 0;
    dataReady.reset();
}
