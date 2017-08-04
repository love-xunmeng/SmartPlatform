#pragma once


#include "boost/shared_ptr.hpp"
#include "boost/thread/thread.hpp"

class DataItem;
class DataQueue;
class Algorithm;

class AlgorithmThread {
public:
	AlgorithmThread(boost::shared_ptr<DataQueue> queue);
	~AlgorithmThread();

	int start();
	void execute(boost::shared_ptr<DataItem> data);
	void stop();

private:
	void process();

private:
	bool is_run_;
	boost::thread thread_;
	boost::shared_ptr<Algorithm> algorithm_;
	boost::shared_ptr<DataQueue> data_queue_;
};