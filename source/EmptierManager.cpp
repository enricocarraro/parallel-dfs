#include "EmptierManager.hpp"

emptierManager::emptierManager(Worker **allWorkers, int nWorkers,
		Semaphore *commonSemQueueFull, Semaphore *commonSemQueueEmpty,
		std::vector<intint> *commonQueue, int graphSize) {
	this->workers = allWorkers;
	this->commonSemQueueFull = commonSemQueueFull;
	this->commonSemQueueEmpty = commonSemQueueEmpty;
	this->nWorkers = nWorkers;
	this->commonQueue = commonQueue;
	this->graphSize = graphSize;
	graph.resize(graphSize);
	for (int i = 0; i < graphSize; i++) {
		graph.at(i) = false;
	}
}

void emptierManager::pushLoop(std::vector<bool> roots) //node containing all other root nodes as neighbours
		{
	intint toPush;
	std::vector<int> *adj;
	toPush.father = -1;

	//adj = &node.adj;
	for (int i = 0; i < roots.size(); i++) {
		if (!roots[i]) {
			toPush.son = i;
			if (!graph.at(toPush.son)/* && toPush.son != toPush.father*/) {
				commonSemQueueEmpty->wait();
				commonQueue->at(queueInsertPosition) = toPush;
				queueInsertPosition = (queueInsertPosition + 1) % (graphSize*queueSize);
				commonSemQueueFull->signal();
				graph.at(toPush.son) = true;
				nodeRead++;
			}
		}
	}
	toPush.son = -1;
	commonSemQueueEmpty->wait();
	commonQueue->at(queueInsertPosition) = toPush;
	queueInsertPosition = (queueInsertPosition + 1) % (graphSize*queueSize);
	commonSemQueueFull->signal();

	int i = 0;
	while (nodeRead < graphSize) {

		workers[i]->askManagerToEmpty->wait();
		toPush.father = workers[i]->neighbours.front().father;
		adj = &workers[i]->neighbours.front().adj;
		for (auto x : *adj) {
			toPush.son = x;
			if (!graph.at(toPush.son)/* && toPush.son != toPush.father*/) {
				commonSemQueueEmpty->wait();
				commonQueue->at(queueInsertPosition) = toPush;
				queueInsertPosition = (queueInsertPosition + 1) % (graphSize*queueSize);
				commonSemQueueFull->signal();
				graph.at(toPush.son) = true;
				nodeRead++;
			}
		}
		toPush.son = -1;
		commonSemQueueEmpty->wait();
		commonQueue->at(queueInsertPosition) = toPush;
		queueInsertPosition = (queueInsertPosition + 1) % (graphSize*queueSize);
		commonSemQueueFull->signal();
		workers[i]->queueExclusion->wait();
		workers[i]->neighbours.pop();
		workers[i]->queueExclusion->signal();

		i = (i + 1) % nWorkers;
		//condizione di stop
	}
}
