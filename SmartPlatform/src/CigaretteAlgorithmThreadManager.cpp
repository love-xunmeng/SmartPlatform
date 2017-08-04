#include "CigaretteAlgorithmThreadManager.h"
#include "boost\function\function0.hpp"
#include "MessageWithSession.h"
#include "MLog.h"
#include "SmartPlatform.pb.h"
#include <iostream>
#include <sstream>
using namespace boost;
using namespace std;


CigaretteAlgorithmThreadManager::CigaretteAlgorithmThreadManager(int cigarette_algorithm_thread_num)
	:cigarette_algorithm_thread_num_(cigarette_algorithm_thread_num)
{
	is_run_ = false;
	message_queue_ = std::shared_ptr<MessageQueue>(new MessageQueue());
	thread_index_ = 0;
}


CigaretteAlgorithmThreadManager::~CigaretteAlgorithmThreadManager()
{
}

int CigaretteAlgorithmThreadManager::start() {
	if (is_run_)
	{
		cerr << "MessageDispatcher is running" << endl;
		return -1;
	}

	for (int i = 0; i < cigarette_algorithm_thread_num_; ++i)
	{
		ostringstream ss;
		ss << "Total CigaretteAlgorithmThread is: " << cigarette_algorithm_thread_num_ << ", thread " << i + 1 << " is starting." << endl;
		MLogger.info(ss.str());

		ss.str("");
		ss << "Thread" << i + 1;

		std::shared_ptr<CigaretteAlgorithmThread> thread = std::shared_ptr<CigaretteAlgorithmThread>(new CigaretteAlgorithmThread(ss.str()));
		thread->start();
		ss.str("");;
		ss << "Total CigaretteAlgorithmThread is " << cigarette_algorithm_thread_num_ << ", thread " << i + 1 << " started." << endl;
		MLogger.info(ss.str());
		cigarette_algorithm_thread_set_.push_back(thread);
	}

	is_run_ = true;
	boost::function0<void> f = boost::bind(&CigaretteAlgorithmThreadManager::dispatch, this);
	thread_ = boost::thread(f);
	return 0;
}

void CigaretteAlgorithmThreadManager::execute(const std::shared_ptr<SmartPlatform::Message> message, const boost::shared_ptr<Session> session) {
	std::shared_ptr<MessageWithSession> message_width_session(new MessageWithSession());
	message_width_session->message_ = message;
	message_width_session->session_ = session;
	message_queue_->push(message_width_session);
}

void CigaretteAlgorithmThreadManager::dispatch() {
	while (is_run_)
	{
		std::shared_ptr<MessageWithSession> message = message_queue_->pop();
		std::shared_ptr<CigaretteAlgorithmThread> thread = cigarette_algorithm_thread_set_[thread_index_];
		thread->execute(message);
		thread_index_ = ++thread_index_ % cigarette_algorithm_thread_set_.size();
	}
}

void CigaretteAlgorithmThreadManager::stop() {
	for each (std::shared_ptr<CigaretteAlgorithmThread> thread in cigarette_algorithm_thread_set_)
	{
		thread->stop();//有可能死锁？当在MessageQueue pop等待起的时候
	}
	if (is_run_)
	{
		is_run_ = false;
		if (thread_.joinable())
		{
			thread_.join();
		}
	}
}