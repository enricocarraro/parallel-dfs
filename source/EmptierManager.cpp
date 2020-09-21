//
// Created by lire on 8/19/20.
//

#include "EmptierManager.h"

using namespace boost::multiprecision;
using namespace std;

emptierManager::emptierManager(vector<Worker> *allWorkers, int nWorkers,
                               Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
                               std::vector<intintint> *commonQueue, Graph *g) {
    this->workers = allWorkers;
    this->commonSemQueueFull = commonSemQueueFull;
    this->commonSemQueueEmpty = commonSemQueueEmpty;
    this->nWorkers = nWorkers;
    this->commonQueue = commonQueue;
    this->graphSize = g->nNodes;
    this->nEdges = g->nEdges;
    this->g = g;
    /*graph.resize(graphSize);
    for (int i = 0; i < graphSize; i++) {
        graph.at(i) = false;
    }*/
    roots = &g->roots;
#if !USE_BOOL
    preLeaves = &g->preLeaves;
#endif
}

void emptierManager::preGraphSize()
{
    int queueInsertPosition = 0;
    int nodeRead = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    //std::vector<int> weights(graphSize, 1);

    intintint toPush;
#if USE_BOOL
    for(int i=0; i<graphSize; i++) {
        if(g->preLeaves.at(i) == true) {
            toPush.father = i;      // the node itself
            toPush.child = 1;       // # of descendants
            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPush;
            commonSemQueueFull->signal();
            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
            nodeRead++;
        }
    }
#else
    for(int i=0; i<preLeaves->size(); i++) {
            toPush.start = preLeaves->at(i);      // the node itself
            toPush.prefix = 1;       // # of descendants
            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPush;
            commonSemQueueFull->signal();
            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
            nodeRead++;
    }
#endif

    int i = 0;
    while(nodeRead<graphSize) {
        workers->at(i).askManagerToEmpty->wait();
        toPush.start = workers->at(i).neighboursWeights.at(positionsIntoWorkQueues[i]).father; //node itself
        toPush.prefix = workers->at(i).neighboursWeights.at(positionsIntoWorkQueues[i]).adjWeights->at(0); //weight
        positionsIntoWorkQueues[i] = (positionsIntoWorkQueues[i]+1)%workers->at(i).graphSize;

        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPush;
        commonSemQueueFull->signal();
        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

        nodeRead++;
        i = (i + 1) % nWorkers;
    }
}


#if !QUICK_TIME_CALC

void emptierManager::pushLoop()
{
    int queueInsertPosition = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    intint toPush;
    intintint toPushWeight;
    std::vector<int> *adj;
    std::vector<uint1024_t> *adjW;
    toPush.father = toPushWeight.father = -1;
    int nodeRead = 0;
    std::vector<int> graph (graphSize, 0);

    uint1024_t weight = 0;

    //adj = &node.adj;
#if USE_BOOL
    for (int i = 0; i < graphSize; i++) {
        if (roots->at(i)) {     //"node" containing all other root nodes as neighbours
            //if (!graph.at(i)/* && toPush.child != toPush.father*/) {
                toPushWeight.child = i;
                toPushWeight.weight = weight;
                commonSemQueueEmpty->wait();
                commonQueue->at(queueInsertPosition) = toPushWeight;
                commonSemQueueFull->signal();

                queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

                graph.at(toPushWeight.child)++;
                weight += g->nodes.at(toPushWeight.child).descendantSize;
                nodeRead++;
            /*}
            else if(toPushWeight.child != toPushWeight.father) {
                toPush.child = toPushWeight.child;
                toPush.father = toPushWeight.father;
                g->cancelledEdges->at(g->posIntoCancelledEdges++) = toPush;
            }*/
        }
    }
#else
    for(int i=0; i<roots->size(); i++) {    //"node" containing all other root nodes as neighbours
        toPushWeight.child = roots->at(i);
        toPushWeight.weight = weight;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPushWeight;
        commonSemQueueFull->signal();

        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

        graph.at(toPushWeight.child)++;
        weight += g->nodes.at(toPushWeight.child).descendantSize;
        nodeRead++;
    }
#endif
    toPushWeight.child = -1;
    commonSemQueueEmpty->wait();
    commonQueue->at(queueInsertPosition) = toPushWeight;
    commonSemQueueFull->signal();
    queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

    int i = 0;
    while (nodeRead < graphSize) {

        workers->at(i).askManagerToEmpty->wait();
        toPushWeight.father = workers->at(i).neighboursWeights.at(positionsIntoWorkQueues[i]).father;
        adj = &workers->at(i).neighboursWeights.at(positionsIntoWorkQueues[i]).adj;
        adjW = workers->at(i).neighboursWeights.at(positionsIntoWorkQueues[i]).adjWeights;
        positionsIntoWorkQueues[i] = (positionsIntoWorkQueues[i]+1)%workers->at(i).graphSize;
        weight = 0;
        for (int j = 0; j<adj->size(); j++) {
            toPushWeight.child = adj->at(j);
            //if(x<0)
            //    cout << "Padre " << toPush.father << " figlio " << x << " potrebbe essere un problema\n";
            //if (graph.at(toPushWeight.child) != g->nodes.at(toPushWeight.child).ancSize) {
                //toPushWeight.weight = 1 + weight + g->nodes.at(toPushWeight.father).fatherWeight;
                toPushWeight.weight = adjW->at(j); //ERROR

                commonSemQueueEmpty->wait();
                commonQueue->at(queueInsertPosition) = toPushWeight; //ERROR 2, con padre 0 ha peso 1, invece  che 3
                commonSemQueueFull->signal();

                queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

                weight += g->nodes.at(toPushWeight.child).descendantSize;
                graph.at(toPushWeight.child)++;
                if (graph.at(toPushWeight.child) == g->nodes.at(toPushWeight.child).ancSize) {
                    nodeRead++;
                }
            /*}
            else if(toPushWeight.child != toPushWeight.father) {
                toPush.child = toPushWeight.child;
                toPush.father = toPushWeight.father;
                g->cancelledEdges->at(g->posIntoCancelledEdges++) = toPush;
            }*/
        }
        /*
        toPushWeight.child = -1;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPushWeight;
        commonSemQueueFull->signal();
        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
         */
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
    intintint toPush;
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
    intintint toPush;
    int nodeRead = 0;
    std::vector<int> *adj;

#if USE_BOOL
    for(int i=0; i<graphSize; i++) {
        if(roots->at(i)) {
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
#else
    for(int i=0; i<roots->size(); i++) {
        toPush.father = roots->at(i);      // the node itself
        toPush.child = time;    // starting instant

        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPush;
        commonSemQueueFull->signal();
        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

        time += (g->nodes.at(toPush.father).subTreeSize);
        nodeRead++;
    }
#endif


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

#else

void emptierManager::weightsAndPrefixes()
{
    int queueInsertPosition = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    intintint toPushWeight;
    int readNodes = 0;

    uint1024_t prefix = 0;
    std::vector<int> *childs;
    boostIntVect *next;

    //adj = &node.adj;
#if USE_BOOL
    printf("NOT WORKING\n");
#else
    for(int i=0; i<roots->size(); i++) {    //"node" containing all other root nodes as neighbours
        toPushWeight.child = roots->at(i);
        toPushWeight.prefix = prefix;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPushWeight;
        commonSemQueueFull->signal();

        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

        prefix += g->nodes.at(toPushWeight.child).nodeWeight;
    }
    readNodes++;
#endif
    int i = 0;
    while (readNodes < graphSize) {

        workers->at(i).askManagerToEmpty->wait();

        next =  &workers->at(i).nextWeights.at(positionsIntoWorkQueues[i]++);
        childs = next->childs;
        prefix = next->prefix;
        for(int j=0; j < childs->size(); j++) {    //"node" containing all other root nodes as neighbours
            toPushWeight.child = childs->at(j);
            toPushWeight.prefix = prefix;
            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPushWeight;
            commonSemQueueFull->signal();

            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);

            prefix += g->nodes.at(childs->at(j)).nodeWeight;
        }
        readNodes++;

        i = (i + 1) % nWorkers;
    }
}


void emptierManager::startEndTimes()
{
    int queueInsertPosition = 0;
    std::vector<int> positionsIntoWorkQueues(nWorkers, 0);
    intintint toPushWeight;
    int readNodes = 0;

    std::vector<int> *fathers;
    intCouple *next;


    for(int i=0; i<preLeaves->size(); i++) {    //"node" containing all other root nodes as neighbours
        toPushWeight.child = preLeaves->at(i);
        toPushWeight.start = g->nodes.at(toPushWeight.child).end;
        commonSemQueueEmpty->wait();
        commonQueue->at(queueInsertPosition) = toPushWeight;
        commonSemQueueFull->signal();

        queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
    }
    readNodes++;


    int i = 0;
    while (readNodes < graphSize) {

        workers->at(i).askManagerToEmpty->wait();

        next =  &workers->at(i).nextStart.at(positionsIntoWorkQueues[i]++);
        toPushWeight.start = next->start;
        fathers = next->fathers;
        for(int j=0; j < fathers->size(); j++) {    //"node" containing all other root nodes as neighbours
            toPushWeight.child = fathers->at(j);
            commonSemQueueEmpty->wait();
            commonQueue->at(queueInsertPosition) = toPushWeight;
            commonSemQueueFull->signal();

            queueInsertPosition = (queueInsertPosition + 1) % (graphSize);
        }
        readNodes++;

        i = (i + 1) % nWorkers;
    }
}

#endif