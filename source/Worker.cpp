#include "Worker.hpp"

Worker::Worker(int id) {
	this->id = id;
}

void Worker::work() {
	Node n;
	//int father;
	intCouple toPush;
	managerHasFed->wait();
	n = next;
	askManagerToFeed->signal();
	while (n.id != -1) {
		toPush.adj = n.adj;
		toPush.father = n.id;
		queueExclusion->wait();
		neighbours.push(toPush);
		queueExclusion->signal();
		askManagerToEmpty->signal();
		managerHasFed->wait();
		n = next;
		askManagerToFeed->signal();
	}
}
