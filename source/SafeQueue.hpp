#ifndef SafeQueue_hpp
#define SafeQueue_hpp

#include <vector>
#include <mutex>
#include <condition_variable>

template <typename T>
class SafeQueue
{
	std::vector<T> q;
	mutable std::mutex m;
	std::condition_variable c;

public:
	SafeQueue();
	SafeQueue<T> &operator=(const SafeQueue<T> &);
	// Add an element to the queue.
	void push(T t);
	size_t size();
	// Get the "front"-element.
	// If the queue is empty, wait till a element is avaiable.
	T pop(void);
	std::vector<T> move_underlying_queue();
};

#endif /* SafeQueue_hpp */
