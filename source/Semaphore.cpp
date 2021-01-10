//
// Created by lire on 8/19/20.
//

#include "Semaphore.hpp"
#include <limits.h>

//constructor
Semaphore::Semaphore(int count, int maxCount)
{
	this->count = count;
	this->maxCount = maxCount;
}
Semaphore::Semaphore(int count)
{
	this->count = count;
	maxCount = INT_MAX;
}
Semaphore::Semaphore()
{
	this->count = 0;
	maxCount = INT_MAX;
}

void Semaphore::wait()
{
	std::unique_lock<std::mutex> lock(mux);
	while (count == 0)
	{
		//cout << "thread " << id << "is waiting"
		cv.wait(lock);
		//cout << "thread " << id << "has finished waiting"
	}
	count--;
}

void Semaphore::signal()
{
	std::unique_lock<std::mutex> lock(mux);
	if (count < maxCount)
	{
		count++;
	}
	//cout << "thread " << id << "is signaling"
	cv.notify_one();
}
