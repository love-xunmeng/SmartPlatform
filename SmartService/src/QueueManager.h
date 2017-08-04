#pragma once

#include "mqueue.h"
#include "boost/shared_ptr.hpp"
#include <vector>

class QueueManager {
public:
	QueueManager()
	:queue_count_(1)
	,index_(0){
		for (int i = 0; i < queue_count_; ++i) {
			queue_vec_.push_back(boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>>(new MQuque<boost::shared_ptr<DataItem>>()));
		}
	}

	~QueueManager() {

	}

	boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>> get_queue() {
		index_ = ++index_ % queue_count_;
		return queue_vec_[index_];
	}

private:
	int queue_count_;
	int index_;
	std::vector<boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>>> queue_vec_;
};
