#include "MLog.h"
#include "ILog.h"
#include "UdpLog.h"
#include "GoogleLog.h"
#include "boost\shared_ptr.hpp"
#include <string>

using namespace boost;
using namespace std;

MLog* MLog::instance_ = NULL;

MLog::MLog()
	:log_(NULL)
{
	//�������ж�ȡlooger_type������ʹ��init������ʹ��init�Ƿ����ã���
	//logger_type_ = logger_type;
	logger_type_ = Google;
	switch (logger_type_) {
	case Google:
		log_ = boost::shared_ptr<ILog>(new GoogleLog);
		break;
	case Udp:
		log_ = boost::shared_ptr<ILog>(new UdpLog);
		break;
	default:
		break;
	}
}

MLog::~MLog()
{
}

MLog& MLog::instance() {
	if (NULL == instance_) {
		instance_ = new MLog();
	}
	return *instance_;
}

void MLog::info(std::string message) {
	log_->info(message);
}

void MLog::warning(std::string message) {
	log_->warning(message);
}

void MLog::error(std::string message) {
	log_->error(message);
}

