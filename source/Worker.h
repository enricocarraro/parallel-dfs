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

public:

    Semaphore *askManagerToEmpty = new Semaphore();
    Semaphore *askManagerToFeed = new Semaphore(1, 1);
    Semaphore *managerHasFed = new Semaphore(0, 1);
    //Semaphore *queueExclusion = new Semaphore(1, 1);

    //Node *separator;
    int getId () { return id; };

    int graphSize;
    std::vector<intCouple> neighbours;
    Node *next;

    //explicit Worker(int id/*, Node *separator*/);
    //Worker();
    void setId(int id) { this->id = id; };
    void initialize(int id, int size, int nWorkers);
    void work();
    void subGraphSize();
    void resetSemaphores();
    void labels();
};

#endif //SDP_PIPELINERESOLUTION_WORKER_H
