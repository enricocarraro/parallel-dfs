#pragma once

#include <queue>
#include <vector>

#include "Graph.hpp"
#include "Semaphore.hpp"

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

	Semaphore *askManagerToEmpty = new Semaphore(0, 1);
	Semaphore *askManagerToFeed = new Semaphore(1, 1);
	Semaphore *managerHasFed = new Semaphore(0, 1);
	Semaphore *queueExclusion = new Semaphore(1, 1);

	//Node *separator;

	std::queue<intCouple> neighbours;
	Node next;

	Worker(int id/*, Node *separator*/);
	void work();
};
