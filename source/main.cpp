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
#include "recalST.h"

using namespace std;

void preGraphSizeWorker(Worker *worker) {
    //cout << "Starting worker " << worker->getId() << "\n";
    worker->preGraphSize();
}

void preGraphSizeEManager(emptierManager eManager) {
    eManager.preGraphSize();
}

void preGraphSizeFManager(feederManager fManager) {
    fManager.preGraphSize();
}

void startWorker(Worker *worker) {
    //cout << "Starting worker " << worker->getId() << "\n";
    worker->work();
}

void startEManager(emptierManager eManager) {
    eManager.pushLoop();
}

void startFManager(feederManager fManager) {
    fManager.feedLoop();
}

void startGraphSizeWorker(Worker *worker) {
    //cout << "Starting worker " << worker->getId() << "\n";
    worker->subGraphSize();
}

void startGraphSizeEManager(emptierManager eManager) {
    eManager.subGraphSize();
}

void startGraphSizeFManager(feederManager fManager) {
    fManager.subGraphSize();
}

void startLabelsWorker(Worker *worker) {
    //cout << "Starting worker " << worker->getId() << "\n";
    worker->labels();
}

void startLabelsEManager(emptierManager eManager) {
    eManager.labels();
}

void startLabelsFManager(feederManager fManager) {
    fManager.labels();
}

void start(int nWorkers, Graph *g) {
    //emptierManager(Worker** allWorkers, int nWorkers, Semaphore* commonSemQueueFull, Semaphore* commonSemQueueEmpty, std::vector<intint> commonQueue, Node *separator, int graphSize);
    //feederManager::feederManager(Worker** allWorkers, int nWorkers, Semaphore* commonSemQueueFull, Semaphore* commonSemQueueEmpty, std::vector<intint> commonQueue, Node *separator, int graphSize, std::vector<Node> *allNodes)
    //Worker::Worker(int id)

    vector<Worker> allWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).initialize(i, g, nWorkers);
    }
    Semaphore commonSemQueueFull (0, g->nNodes);
    Semaphore commonSemQueueEmpty (g->nNodes, g->nNodes);
    std::vector<intintint> commonQueue (g->nNodes);
    emptierManager eManager(&allWorkers, nWorkers, &commonSemQueueFull,
                            &commonSemQueueEmpty, &commonQueue, g);
    feederManager fManager(&allWorkers, nWorkers, &commonSemQueueFull,
                           &commonSemQueueEmpty, &commonQueue, g);


    //pre phase

    vector<thread> tPreGraphSizeWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        tPreGraphSizeWorkers[i] = thread(preGraphSizeWorker, &allWorkers.at(i));
    }
    thread tPreGraphSizeEManager(preGraphSizeEManager, eManager);
    thread tPreGraphSizeFManager(preGraphSizeFManager, fManager);

    for (int i = 0; i < nWorkers; i++) {
        tPreGraphSizeWorkers[i].join();
    }
    tPreGraphSizeEManager.join();
    tPreGraphSizeFManager.join();


    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).resetSemaphores();
    }
    commonSemQueueEmpty.reset();
    commonSemQueueFull.reset();


    //first phase

    vector<thread> tWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        tWorkers[i] = thread(startWorker, &allWorkers.at(i));
    }
    thread tEManager(startEManager, eManager);
    thread tFManager(startFManager, fManager);
    //std::thread first (foo);

    for (int i = 0; i < nWorkers; i++) {
        tWorkers[i].join();
    }
    tEManager.join();
    tFManager.join();


    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).resetSemaphores();
    }
    commonSemQueueEmpty.reset();
    commonSemQueueFull.reset();

    //resize of cancelledEdges
    //g->cancelledEdges->resize(g->posIntoCancelledEdges);

    //second phase

    vector<thread> tGraphSizeWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        tGraphSizeWorkers[i] = thread(startGraphSizeWorker, &allWorkers.at(i));
    }
    thread tGraphSizeEManager(startGraphSizeEManager, eManager);
    thread tGraphSizeFManager(startGraphSizeFManager, fManager);

    for (int i = 0; i < nWorkers; i++) {
        tGraphSizeWorkers[i].join();
    }
    tGraphSizeEManager.join();
    tGraphSizeFManager.join();


    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).resetSemaphores();
    }
    commonSemQueueEmpty.reset();
    commonSemQueueFull.reset();


    //third phase

    vector<thread> tLabelsWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        tLabelsWorkers[i] = thread(startLabelsWorker, &allWorkers.at(i));
    }
    thread tLabelsEManager(startLabelsEManager, eManager);
    thread tLabelsFManager(startLabelsFManager, fManager);

    for (int i = 0; i < nWorkers; i++) {
        tLabelsWorkers[i].join();
    }
    tLabelsEManager.join();
    tLabelsFManager.join();

    recalST rST;
    rST.init(g);
    rST.work();

}

#define QUICK_RUN 1
#define FILE_N 4

int main(int argc, const char *argv[]) {
    FILE *fp;

#if QUICK_RUN
    string graname;
    switch (FILE_N) {
        case 0:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v4e2.gra");
            break;
        case 1:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v10e3.gra");
            break;
        case 2:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v10e3v2.gra");
            break;
        case 3:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v5000e50.gra");
            break;
        case 4:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v100000e100.gra");
            break;
    }
#else
    // 1 parameter of format .gra is required.
    if (argc != 2) {
        cout << "Error: Missing parameter" << endl;
        return -1;
    }
    string graname(argv[1]);
#endif

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
    //g.printTrueGraphSize();
    //g.printTrueLabels();
    //g.printTrueLabelsPreWeights();      //prints everything

    return 0;

}
