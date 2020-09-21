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
    int start;
    std::vector<int> *fathers;
};
struct intVetVet {
    int father;
    std::vector<int> adj;
    std::vector<boost::multiprecision::uint1024_t> *adjWeights;
};
struct boostIntVect {
    boost::multiprecision::uint1024_t prefix;
    std::vector<int> *childs;
};


class Worker {

    int id;

public:

    Semaphore *askManagerToEmpty = new Semaphore();
    Semaphore *askManagerToFeed = new Semaphore(1, 1);
    Semaphore *managerHasFed = new Semaphore(0, 1);
    //Semaphore *queueExclusion = new Semaphore(1, 1);

    //Node *separator;
    int getId () { return id; };

    Graph *g;
    int graphSize;
    std::vector<intCouple> nextStart;
    std::vector<intVetVet> neighboursWeights;
    std::vector<boostIntVect> nextWeights;
    Node *next;

    //explicit Worker(int id/*, Node *separator*/);
    //Worker();
    void setId(int id) { this->id = id; };
    void initialize(int id, Graph *g, int nWorkers);
    void work();
    void subGraphSize();
    void preGraphSize();
    void resetSemaphores();
    void labels();
    void weightsAndPrefixes();
    void startEndTimes();
};

#endif //SDP_PIPELINERESOLUTION_WORKER_H
