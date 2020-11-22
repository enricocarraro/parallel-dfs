//
// Created by lire on 10/3/20.
//

#include "SecureQueue.h"

SecureQueue::SecureQueue(int queueSize, int nWorker) { //maybe nThread is not usefull
    this->queueSize = queueSize + nWorker;
    this->queue.resize(this->queueSize);
    this->control.resize(this->queueSize, false);
    this->controlN.resize(this->queueSize, 0);
#if OPT_ATOMIC
    this->barrier.resize(this->queueSize+1, false);
#endif
    insertPosition = 0;
    extractPosition = 0;
}

int SecureQueue::push(int next) {
#if OPT_ATOMIC
    int pos = insertPosition.fetch_add(1);
    queue.at(pos) = next;
    while(!barrier.at(pos)) {}
    dataReady.signal();
    barrier.at(pos+1) = true;
#else
    int pos = insertPosition++;
    queue.at(pos) = next;

    control.at(pos) = (control.at(pos) != true);

    dataReady.signal();
#endif
    return pos;
}

int SecureQueue::pop() {
    dataReady.wait();
#if OPT_ATOMIC
    int pos = extractPosition.fetch_add(1);
#else
    //bs.wait();
    int pos = extractPosition++;
    //bs.signal();
#endif
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
#if OPT_ATOMIC
    for (int i=1; i<this->barrier.size(); i++) {
        barrier.at(i) = false;
    }
#endif
}
