#pragma once

#include "mqueue.h"
#include "Session.h"
#include "SmartPlatform.pb.h"
#include "boost/shared_ptr.hpp"
#include <memory>

class MessageWithSession
{
public:
	MessageWithSession();
	~MessageWithSession();

public:
	std::shared_ptr<SmartPlatform::Message> message_;
	//std::shared_ptr<Session> session_;
	boost::shared_ptr<Session> session_;
};

