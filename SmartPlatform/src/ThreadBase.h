#pragma once

#include "mqueue.h"
#include "opencv2\opencv.hpp"
#include "boost/thread/thread.hpp"
#include <string>

class ThreadBase
{
public:
	ThreadBase(std::string thread_name);
	~ThreadBase();

private:
	bool is_run_ = false;
	boost::thread thread_;
	std::string thread_name_ = "";
};

