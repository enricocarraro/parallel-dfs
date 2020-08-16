#include <iostream>
#include <stdio.h>
#include "Semaphore.hpp"
#include <thread>

#include "EmptierManager.hpp"
#include "FeederManager.hpp"
#include "Graph.hpp"
#include "Timer.cpp"
#include "Worker.hpp"

using namespace std;

void startWorker(Worker *worker) {
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

	Worker *allWorkers[nWorkers];
	for (int i = 0; i < nWorkers; i++) {
		allWorkers[i] = new Worker(i);
	}
	Semaphore *commonSemQueueFull = new Semaphore(0, g->size() * queueSize);
	Semaphore *commonSemQueueEmpty = new Semaphore(g->size() * queueSize,
			g->size() * queueSize);
	std::vector<intint> *commonQueue = new std::vector<intint>(
			g->size() * queueSize);
	emptierManager eManager(allWorkers, nWorkers, commonSemQueueFull,
			commonSemQueueEmpty, commonQueue, g->size());
	feederManager fManager(allWorkers, nWorkers, commonSemQueueFull,
			commonSemQueueEmpty, commonQueue, g);

	vector<thread> tWorkers(nWorkers);
	for (int i = 0; i < nWorkers; i++) {
		//tWorkers[i] = new thread (startWorker, allWorkers[i]);
		tWorkers[i] = thread(startWorker, allWorkers[i]);
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
	if (argc != 2) {
		cout << "Error: Missing parameter" << endl;
		return -1;
	}
	string graname(argv[1]);

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
