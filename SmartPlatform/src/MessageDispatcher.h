#pragma once

#include "Common.h"
#include "IMessageHandler.h"
#include "SmartPlatform.pb.h"
#include <boost/thread/thread.hpp>
#include <map>
#include <memory>

class MessageDispatcher
{
public:
	explicit MessageDispatcher(RawMessageDataQueuePtr raw_message_data_queue);
	~MessageDispatcher();

	int start();
	int register_handler(SmartPlatform::MSG type, IMessageHandler *handler);
	void stop();

private:
	void dispatch();
	std::shared_ptr<SmartPlatform::Message> parse(char *buffer, int buffer_size);

private:
	bool is_run_;
	std::map<SmartPlatform::MSG, IMessageHandler*> handler_set_;
	RawMessageDataQueuePtr raw_message_data_queue_;
	boost::thread thread_;
};

