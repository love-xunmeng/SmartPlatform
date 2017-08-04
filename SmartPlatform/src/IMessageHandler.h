#pragma once

#include "SmartPlatform.pb.h"
#include "Session.h"
#include "boost/shared_ptr.hpp"
#include <memory>

class IMessageHandler
{
public:
	IMessageHandler();
	~IMessageHandler();

	virtual void execute(const std::shared_ptr<SmartPlatform::Message> msg, const boost::shared_ptr<Session> session) = 0;
	//virtual void execute(const std::shared_ptr<SmartPlatform::Message> msg, const std::shared_ptr<Session> session) = 0;
};

