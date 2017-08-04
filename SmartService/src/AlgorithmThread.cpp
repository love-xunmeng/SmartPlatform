//#include "AlgorithmThread.h"
//#include "Algorithm.h"
//#include "DataItem.h"
//#include "Session.h"
//#include "boost/function/function0.hpp"
//#include <sstream>
//#include <time.h>
//
//using namespace boost;
//using namespace std;
//
//AlgorithmThread::AlgorithmThread(boost::shared_ptr<DataQueue> queue)
//	: data_queue_(queue)
//	, is_run_(false)
//	, algorithm_(new Algorithm()) {
//
//}
//
//AlgorithmThread::~AlgorithmThread() {
//
//}
//
//int AlgorithmThread::start() {
//	is_run_ = true;
//	boost::function0<void> f = boost::bind(&AlgorithmThread::process, this);
//	thread_ = boost::thread(f);
//	return 0;
//}
//
//void AlgorithmThread::stop() {
//	if (is_run_) {
//		is_run_ = false;
//		if (thread_.joinable()) {
//			thread_.join();
//		}
//	}
//}
//
//void AlgorithmThread::execute(boost::shared_ptr<DataItem> data_item) {
//	data_queue_->push(data_item);
//}
//
//void AlgorithmThread::process() {
//	while (is_run_) {
//		boost::shared_ptr<DataItem> data_item = data_queue_->pop();
//		//result
//		static int i = 1;
//		ostringstream ss;
//		ss << "{\"id\":" << i << "}";
//		string str_result = ss.str();
//		cout << "str_result: " << str_result << endl;
//		data_item->session_->write(str_result);
//	}
//}
