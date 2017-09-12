#ifndef mqueue_h_
#define mqueue_h_

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"
#include <iostream>
#include <queue>

template<class T>
class MQuque {
public:
	MQuque(){}
	~MQuque(){}

	T pop() {
		boost::mutex::scoped_lock lock(mutex_);
		if (0 == queue_.size())
		{
			condition_pop_.wait(lock);
		}
		if (0 == queue_.size()) {
			int i = 0;
		}
		T element = queue_.front();
		queue_.pop();
		return element;
	}

	void push(T element) {
		boost::mutex::scoped_lock lock(mutex_);
		queue_.push(element);
		if (1 == queue_.size())
		{
			condition_pop_.notify_one();
		}
	}

private:
	std::queue<T> queue_;
	boost::mutex mutex_;
	boost::condition condition_pop_;
};

#endif // mqueue_h_
