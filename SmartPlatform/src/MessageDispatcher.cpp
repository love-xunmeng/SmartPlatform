#include "CigaretteAlgorithmThreadManager.h"
#include "IMessageHandler.h"
#include "MessageDispatcher.h"
#include "RawDataWithSession.h"
#include "SmartPlatform.pb.h"
#include "boost/function/function0.hpp"
#include <iostream>
using namespace std;

MessageDispatcher::MessageDispatcher(RawMessageDataQueuePtr raw_message_data_queue)
	:raw_message_data_queue_(raw_message_data_queue)
{
	is_run_ = false;
}


MessageDispatcher::~MessageDispatcher()
{
}

int MessageDispatcher::start() {
	if (is_run_)
	{
		cerr << "MessageDispatcher is running" << endl;
		return -1;
	}
	is_run_ = true;
	boost::function0<void> f = boost::bind(&MessageDispatcher::dispatch, this);
	thread_ = boost::thread(f);
	return 0;
}

int MessageDispatcher::register_handler(SmartPlatform::MSG type, IMessageHandler *handler) {
	IMessageHandler *tmp = handler_set_[type];
	if (nullptr == tmp)
	{
		handler_set_[type] = handler;
	}
	else
	{
		cerr << "Error: handler " << type << " has existed" << endl;
		return -1;
	}
	return 0;
}

void MessageDispatcher::stop() {
	is_run_ = false;
	thread_.join();
}

void MessageDispatcher::dispatch() {
	while (is_run_)
	{
		std::shared_ptr<RawDataWithSession> raw_data = raw_message_data_queue_->pop();
		boost::shared_ptr<Session> session = raw_data->session_;

		std::shared_ptr<SmartPlatform::Message> message = parse(raw_data->raw_data_->data(), raw_data->raw_data_->data_size());
		if (nullptr == message) {
			SmartPlatform::Message message;
			message.set_version_code("V0.00001");
			message.set_type(SmartPlatform::CigaretteAlgorithm_Response);
			message.set_sequence(1);
			SmartPlatform::Response *response = message.mutable_response();
			response->set_state_code(SmartPlatform::BadRequest);
			response->set_state_desc("Bad request protobuf data.");
			std::string str_message;
			message.SerializeToString(&str_message);
			session->write(str_message);
			continue;
		}
		
		IMessageHandler *observer = handler_set_[message->type()];
		if (observer)
		{
			observer->execute(message, session);
		}
		else
		{
			SmartPlatform::Message message;
			message.set_version_code("V0.00001");
			message.set_type(SmartPlatform::CigaretteAlgorithm_Response);
			message.set_sequence(1);
			SmartPlatform::Response *response = message.mutable_response();
			response->set_state_code(SmartPlatform::BadRequest);
			response->set_state_desc("Requested service does not exist.");
			std::string str_message;
			message.SerializeToString(&str_message);
			session->write(str_message);
		}
	}
}

std::shared_ptr<SmartPlatform::Message> MessageDispatcher::parse(char *buffer, int buffer_size) {
	std::shared_ptr<SmartPlatform::Message> message(new SmartPlatform::Message());
	bool ret = message->ParseFromArray(buffer, buffer_size);
	if (ret) {
		return message;
	}
	return nullptr;
}
