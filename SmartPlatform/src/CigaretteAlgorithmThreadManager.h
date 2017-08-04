#ifndef message_dispatcher_h_
#define message_dispatcher_h_

#include "CigaretteAlgorithmThread.h"
#include "Common.h"
#include "IMessageHandler.h"
#include "boost/thread/thread.hpp"
#include <memory>
#include <vector>

class CigaretteAlgorithmThreadManager : public IMessageHandler
{
public:
	explicit CigaretteAlgorithmThreadManager(int cigarette_algorithm_thread_num);
	~CigaretteAlgorithmThreadManager();

	int start();
	virtual void execute(const std::shared_ptr<SmartPlatform::Message> msg, const boost::shared_ptr<Session> session);
	void stop();

private:
	void dispatch();

private:
	bool is_run_;
	int cigarette_algorithm_thread_num_;
	boost::thread thread_;
	std::vector< std::shared_ptr<CigaretteAlgorithmThread> > cigarette_algorithm_thread_set_;
	MessageQueuePtr message_queue_;
	int thread_index_;
};

#endif // message_dispatcher_h_


