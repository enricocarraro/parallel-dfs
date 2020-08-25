//
// Created by lire on 8/19/20.
//

#include "EmptierManager.h"

using namespace std;

emptierManager::emptierManager(vector<Worker> *allWorkers, int nWorkers,
                               Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                               std::vector<intint> *commonQueue, Graph *g) {
    this->workers = allWorkers;
    this->commonSemQueueFull = commonSemQueueFull;
    this->commonSemQueueEmpty = commonSemQueueEmpty;
    this->nWorkers = nWorkers;
    this->commonQueue = commonQueue;
    this->graphSize = g->size();
    this->g = g;
    graph.resize(graphSize);
    for (int i = 0; i < graphSize; i++) {
        graph.at(i) = false;
    }
    roots = g->returnRoots();
}

void emptierManager::pushLoop() //node containing all other root nodes as neighbours
{
    int queueInsertPosition = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    intint toPush;
    std::vector<int> *adj;
    toPush.father = -1;
    int nodeRead = 0;

    //adj = &node.adj;
    for (int i = 0; i < graphSize; i++) {
        if (!roots.at(i)) {
            if (!graph.at(i)/* && toPush.child != toPush.father*/) {
                toPush.child = i;
                commonSemQueueEmpty->wait();
                commonQueue->at(queueInsertPosition) = toPush;
                commonSemQueueFull->signal();
                queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
                graph.at(toPush.child) = true;
                nodeRead++;
            }
            else if(toPush.child != toPush.father) {
                g->cancelledEdges->push_back(toPush);
            }
        }
    }
    toPush.child = -1;
    commonSemQueueEmpty->wait();
    commonQueue->at(queueInsertPosition) = toPush;
    commonSemQueueFull->signal();
    queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

    int i = 0;
    while (nodeRead < graphSize) {

        workers->at(i).askManagerToEmpty->wait();
        toPush.father = workers->at(i).neighbours.at(positionsIntoWorkQueues[i]).father;
        adj = &workers->at(i).neighbours.at(positionsIntoWorkQueues[i]).adj;
        positionsIntoWorkQueues[i] = (positionsIntoWorkQueues[i]+1)%workers->at(i).graphSize;
        for (auto x : *adj) {
            toPush.child = x;
            //if(x<0)
            //    cout << "Padre " << toPush.father << " figlio " << x << " potrebbe essere un problema\n";
            if (!graph.at(toPush.child)) {
                commonSemQueueEmpty->wait();
                commonQueue->at(queueInsertPosition) = toPush;
                commonSemQueueFull->signal();
                queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
                graph.at(toPush.child) = true;
                nodeRead++;
            }
            else if(toPush.child != toPush.father) {
                g->cancelledEdges->push_back(toPush);
            }
        }
        toPush.child = -1;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPush;
        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
        commonSemQueueFull->signal();
        //workers->at(i).queueExclusion->wait();
        //workers->at(i).neighbours.pop();
        //workers->at(i).queueExclusion->signal();

        i = (i + 1) % nWorkers;
    }
}

void emptierManager::subGraphSize()
{
    int queueInsertPosition = 0;
    int nodeRead = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    intint toPush;
    for(int i=0; i<graphSize; i++) {
        if(g->leaves.at(i) == true) {
            toPush.father = i;      // the node itself
            toPush.child = 1;       // # of descendants
            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPush;
            commonSemQueueFull->signal();
            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
            nodeRead++;
        }
    }

    int i = 0;
    while(nodeRead<graphSize) {
        workers->at(i).askManagerToEmpty->wait();

        toPush.father = workers->at(i).neighbours.at(positionsIntoWorkQueues[i]).father;
        toPush.child = workers->at(i).neighbours.at(positionsIntoWorkQueues[i]).adj.at(0);
        positionsIntoWorkQueues[i] = (positionsIntoWorkQueues[i]+1)%workers->at(i).graphSize;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPush;
        commonSemQueueFull->signal();
        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
        nodeRead++;

        i = (i + 1) % nWorkers;
    }
}

void emptierManager::labels() {
    int queueInsertPosition = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    int time = 1;
    intint toPush;
    int nodeRead = 0;
    std::vector<int> *adj;


    for(int i=0; i<graphSize; i++) {
        if(!roots.at(i)) {
            toPush.father = i;      // the node itself
            toPush.child = time;    // starting instant

            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPush;
            commonSemQueueFull->signal();
            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

            time += (g->nodes.at(i).subTreeSize);
            nodeRead++;
        }
    }
    int i = 0;
    while(nodeRead<graphSize) {
        workers->at(i).askManagerToEmpty->wait();
        time = workers->at(i).neighbours.at(positionsIntoWorkQueues[i]).father;
        adj = &workers->at(i).neighbours.at(positionsIntoWorkQueues[i]).adj;
        positionsIntoWorkQueues[i] = (positionsIntoWorkQueues[i]+1)%workers->at(i).graphSize;

        for(auto x : *adj) {
            toPush.father = x;      // the node itself
            toPush.child = time;    //starting time

            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPush;
            commonSemQueueFull->signal();
            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

            time += (g->nodes.at(x).subTreeSize);
            nodeRead++;
        }

        i = (i + 1) % nWorkers;
    }
}