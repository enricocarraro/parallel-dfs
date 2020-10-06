//
// Created by lire on 8/19/20.
//

#ifndef SDP_PIPELINERESOLUTION_WORKER_H
#define SDP_PIPELINERESOLUTION_WORKER_H

//#include <queue>
#include <vector>

#include "Graph.h"
#include "OPTIONS.h"
#if USE_QUICK_SEM
#include "FastSemaphore.h"
#else
#include "Semaphore.h"
#endif
#include "SecureQueue.h"

/*struct NodeWrapper
 {
 int father;
 struct Node n;
 };*/

#if !LIMITED_SPACE
struct intCouple {
    int father;
    std::vector<int> *adj;
};
struct intVetVet {
    int father;
    std::vector<int> *adj;
    std::vector<boost::multiprecision::uint1024_t> *adjWeights;
};
struct boostIntVect {
    boost::multiprecision::uint1024_t father;
    std::vector<int> *adj;
};
#else
struct boostIntVectBoostVect {
    boost::multiprecision::uint1024_t father;
    std::vector<int> *adj;
    std::vector<boost::multiprecision::uint1024_t> *adjWeights;
};
#endif

struct sharedData {
    Graph *g;
    SecureQueue sq;
    std::vector<std::atomic<int>> visitedNeighbours;
    std::atomic<int> nodeRead = 0;
    sharedData(Graph *gra, int nWorker): g(gra), sq(gra->nNodes, nWorker), visitedNeighbours(gra->nNodes) {
        for(int i=0; i<gra->nNodes; i++) {
            visitedNeighbours.at(i) = 0;
        }
    };
};


class Worker {

    int id;
    //SecureQueue *sq;

public:
#if !USE_QUICK_SEM
    Semaphore *askManagerToEmpty = new Semaphore();
    Semaphore *managerHasEmptied;
    Semaphore *askManagerToFeed = new Semaphore(1, 1);
    Semaphore *managerHasFed = new Semaphore(0, 1);
#else
    FastSemaphore *askManagerToEmpty = new FastSemaphore();
    FastSemaphore *managerHasEmptied;
    FastSemaphore *askManagerToFeed = new FastSemaphore(1);
    FastSemaphore *managerHasFed = new FastSemaphore();
#endif

    //Node *separator;
    int getId() { return id; };

    Graph *g;
    int graphSize;
#if !LIMITED_SPACE
    std::vector<intCouple> nextStart;
    std::vector<intVetVet> neighboursWeights;
    std::vector<boostIntVect> nextWeights;
#else
    std::vector<boostIntVectBoostVect> results;
#endif
    Node *next;

    sharedData *sd;


    void initialize(int id, int nWorkers, sharedData *sd);

    void nodeSize();

    void reset();

    void nodeWeights(bool works_on_roots);

    void nodeTimes();
};

#endif //SDP_PIPELINERESOLUTION_WORKER_H
