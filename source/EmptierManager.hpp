#pragma once
#include <queue>
#include <vector>

#include "FeederManager.hpp"
#include "Worker.hpp"

class emptierManager {
	Worker **workers;
	Semaphore *commonSemQueueFull;
	Semaphore *commonSemQueueEmpty;
	int queueInsertPosition = 0;
	std::vector<struct intint> *commonQueue;
	int nWorkers;
	//Node *separator;
	int graphSize;
	std::vector<bool> graph;
	int nodeRead = 0;

public:
	emptierManager(Worker **allWorkers, int nWorkers,
			Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
			std::vector<intint> *commonQueue,/* Node *separator,*/
			int graphSize); //commonQueue must have size equal to graphSize
	void pushLoop(std::vector<bool> roots);
};
