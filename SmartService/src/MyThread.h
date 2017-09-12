#pragma once

#include "DataItem.h"
#include "IAlgorithm.h"
#include "mqueue.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/function/function0.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/thread/thread.hpp"
#include <sstream>
#include <string>

class MyThread {
public:
	MyThread(boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>> queue, boost::shared_ptr<IAlgorithm> algorithm)
		: queue_(queue)
		, algorithm_(algorithm)
		, is_run_(false) {

	}

	~MyThread() {
		stop();
	}

	int start() {
		is_run_ = true;
		boost::function0<void> f = boost::bind(&MyThread::process, this);
		thread_ = boost::thread(f);
		return 0;
	}

	void stop() {
		if (is_run_) {
			is_run_ = false;
			if (thread_.joinable()) {
				//thread_.join();
				thread_.timed_join(boost::posix_time::microseconds(100));
			}
		}
	}

private:
	void process() {
		while (is_run_) {
			boost::shared_ptr<DataItem> data_item = queue_->pop();
			std::string str_result = algorithm_->process(data_item->json_data_);
			data_item->session_->write(str_result);
		}
	}

private:
	bool is_run_;
	boost::thread thread_;
	boost::shared_ptr<IAlgorithm> algorithm_;
	boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>> queue_;
};
