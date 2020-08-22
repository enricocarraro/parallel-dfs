#include <iostream>
#include <stdio.h>
#include <thread>
#include <vector>

#include "EmptierManager.h"
#include "FeederManager.h"
#include "Graph.h"
#include "Timer.cpp"
#include "Worker.h"
#include "Semaphore.h"

using namespace std;

void startWorker(Worker *worker) {
    //cout << "Starting worker " << worker->getId() << "\n";
    worker->work();
}

void startEManager(emptierManager eManager, std::vector<bool> roots) {
    eManager.pushLoop(roots);
}

void startFManager(feederManager fManager) {
    fManager.feedLoop();
}

void start(int nWorkers, Graph *g) {
    //emptierManager(Worker** allWorkers, int nWorkers, Semaphore* commonSemQueueFull, Semaphore* commonSemQueueEmpty, std::vector<intint> commonQueue, Node *separator, int graphSize);
    //feederManager::feederManager(Worker** allWorkers, int nWorkers, Semaphore* commonSemQueueFull, Semaphore* commonSemQueueEmpty, std::vector<intint> commonQueue, Node *separator, int graphSize, std::vector<Node> *allNodes)
    //Worker::Worker(int id)

    vector<Worker> allWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).setId(i);
    }
    Semaphore commonSemQueueFull = Semaphore(0, g->size() * queueSize);
    Semaphore commonSemQueueEmpty = Semaphore(g->size() * queueSize,
                                                   g->size() * queueSize);
    std::vector<intint> commonQueue = std::vector<intint>(
            g->size() * queueSize);
    emptierManager eManager(&allWorkers, nWorkers, &commonSemQueueFull,
                            &commonSemQueueEmpty, &commonQueue, g->size());
    feederManager fManager(&allWorkers, nWorkers, &commonSemQueueFull,
                           &commonSemQueueEmpty, &commonQueue, g);

    vector<thread> tWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        tWorkers[i] = thread(startWorker, &allWorkers.at(i));
    }
    thread tEManager(startEManager, eManager, g->returnRoots());
    thread tFManager(startFManager, fManager);
    //std::thread first (foo);

    for (int i = 0; i < nWorkers; i++) {
        tWorkers[i].join();
    }
    tEManager.join();
    tFManager.join();
}

int main(int argc, const char *argv[]) {
    FILE *fp;

    // 1 parameter of format .gra is required.
    /*if (argc != 2) {
        cout << "Error: Missing parameter" << endl;
        return -1;
    }
    string graname(argv[1]);*/

    string graname("/home/lire/CLionProjects/sdp_pipelineSolution/v5000e50.gra");

    if ((fp = fopen(graname.c_str(), "r")) == NULL) {
        cout << "Error: File doesn't exist." << endl;
        return -1;
    }

    Timer t;
    t.start();

    Graph g(fp);
    fclose(fp);

    g.sortVectors();
    t.stop();
    t.printElapsed();

    t.start();
    start(2, &g);
    t.stop();
    t.printElapsed();

    //g.printTrueGraph();
    return 0;

}