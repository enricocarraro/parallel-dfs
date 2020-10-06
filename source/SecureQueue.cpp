//
// Created by lire on 10/3/20.
//

#include "SecureQueue.h"

SecureQueue::SecureQueue(int queueSize, int nWorker) { //maybe nThread is not usefull
    this->queueSize = queueSize + nWorker;
    this->queue.resize(this->queueSize);
    insertPosition = 0;
    extractPosition = 0;
}

int SecureQueue::push(int next) {
    bs.wait();
    int pos = insertPosition.fetch_add(1);
    queue.at(pos) = next;
    bs.signal();
    dataReady.signal();
    return pos;
}

int SecureQueue::pop() {
    dataReady.wait();
    int pos = extractPosition.fetch_add(1);
    return queue.at(pos);
}

void SecureQueue::reset() {
    insertPosition = 0;
    extractPosition = 0;
    dataReady.reset();
}
