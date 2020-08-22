//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_WORKER_H
#define SDP_PIPELINERESOLUTION_WORKER_H

#include <queue>
#include <vector>

#include "Graph.h"
#include "Semaphore.h"

/*struct NodeWrapper
 {
 int father;
 struct Node n;
 };*/

struct intCouple {
    int father;
    std::vector<int> adj;
};

#define workerSwitchSemToken 1
class Worker {

    int id;
    int positionIntoGraphVector = 0;

public:

    Semaphore askManagerToEmpty = Semaphore();
    int graphSize;
    //Semaphore managerHasEmptied = Semaphore();
    Semaphore askManagerToFeed = Semaphore(1, 1);
    Semaphore managerHasFed = Semaphore(0, 1);
    //Semaphore queueExclusion = Semaphore(1, 1);

    //Node *separator;
    int getId () { return id; };

    std::vector<intCouple> neighbours;
    Node next;

    //explicit Worker(int id/*, Node *separator*/);
    //Worker(int id, int graphSize);
    void setId(int id) { this->id = id; };
    void setGraphSize(int size) { graphSize = size; neighbours.resize(graphSize); };
    void initialize(int id, int size, int nWorkers);
    void work();
};

#endif //SDP_PIPELINERESOLUTION_WORKER_H
