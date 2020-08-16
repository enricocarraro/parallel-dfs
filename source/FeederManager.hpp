#pragma once
#include <queue>
#include <vector>
#include "Graph.hpp"
#include "Worker.hpp"

#define queueSize 1

struct intint {
	int father;
	int son;
};

class feederManager {
	Worker **workers;
	Semaphore *commonSemQueueFull;
	Semaphore *commonSemQueueEmpty;
	int queueExtractPosition = 0;
	std::vector<struct intint> *commonQueue;
	int nWorkers;
	Node *separator;
	int graphSize;
	Graph *g;
	int nodeRead = 0;

public:
	feederManager(Worker **allWorkers, int nWorkers,
			Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
			std::vector<intint> *commonQueue,/* Node *separator,*/Graph *g); //commonQueue must have size equal to graphSize
	void feedLoop();
};
