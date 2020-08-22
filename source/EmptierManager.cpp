//
// Created by lire on 8/19/20.
//

#include "EmptierManager.h"

using namespace std;

emptierManager::emptierManager(vector<Worker> *allWorkers, int nWorkers,
                               Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                               std::vector<intint> *commonQueue, int graphSize) {
    this->workers = allWorkers;
    this->commonSemQueueFull = commonSemQueueFull;
    this->commonSemQueueEmpty = commonSemQueueEmpty;
    this->nWorkers = nWorkers;
    this->commonQueue = commonQueue;
    this->graphSize = graphSize;
    graph.resize(graphSize);
    for (int i = 0; i < graphSize; i++) {
        graph.at(i) = false;
    }
    positionsIntoWorkQueues.resize(graphSize);
    for (int i = 0; i < graphSize; i++) {
        positionsIntoWorkQueues.at(i) = 0;
    }
}

void emptierManager::pushLoop(std::vector<bool> roots) //node containing all other root nodes as neighbours
{
    intint toPush;
    std::vector<int> *adj;
    toPush.father = -1;

    //adj = &node.adj;
    for (int i = 0; i < roots.size(); i++) {
        if (!roots[i]) {
            toPush.son = i;
            if (!graph.at(toPush.son)/* && toPush.son != toPush.father*/) {
                commonSemQueueEmpty->wait();
                commonQueue->at(queueInsertPosition) = toPush;
                queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
                commonSemQueueFull->signal();
                graph.at(toPush.son) = true;
                nodeRead++;
            }
        }
    }
    toPush.son = -1;
    commonSemQueueEmpty->wait();
    commonQueue->at(queueInsertPosition) = toPush;
    queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
    commonSemQueueFull->signal();

    int i = 0;
    while (nodeRead < graphSize) {

        workers->at(i).askManagerToEmpty.wait();
        toPush.father = workers->at(i).neighbours.at(positionsIntoWorkQueues[i]).father;
        adj = &workers->at(i).neighbours.at(positionsIntoWorkQueues[i]).adj;
        positionsIntoWorkQueues[i] = (positionsIntoWorkQueues[i]+1)%workers->at(i).graphSize;
        for (auto x : *adj) {
            toPush.son = x;
            if(x<0)
                cout << "Padre " << toPush.father << " figlio " << x << " potrebbe essere un problema\n";
            if (!graph.at(toPush.son)/* && toPush.son != toPush.father*/) {
                commonSemQueueEmpty->wait();
                commonQueue->at(queueInsertPosition) = toPush;
                queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
                commonSemQueueFull->signal();
                graph.at(toPush.son) = true;
                nodeRead++;
            }
        }
        toPush.son = -1;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPush;
        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
        commonSemQueueFull->signal();
        /*workers->at(i).queueExclusion->wait();
        workers->at(i).neighbours.pop();
        workers->at(i).queueExclusion->signal();*/

        i = (i + 1) % nWorkers;
        //condizione di stop
    }
}