#include "GoogleLog.h"

#include "glog\logging.h"
#include <string>
using namespace std;

static inline void signal_handle(const char* data, int size) {
	std::string str = std::string(data, size);
	LOG(ERROR) << str;
}

GoogleLog::GoogleLog()
{
	FLAGS_log_dir = "./data/log";
	google::InitGoogleLogging("SmartPlatform");
	google::SetStderrLogging(google::GLOG_INFO);
	FLAGS_colorlogtostderr = true;
	FLAGS_logbufsecs = 0;
	FLAGS_stop_logging_if_full_disk = true;
	google::InstallFailureSignalHandler();
	google::InstallFailureWriter(&signal_handle);
}


GoogleLog::~GoogleLog()
{
}

void GoogleLog::info(std::string message) {
	LOG(INFO) << message;
	
}

void GoogleLog::warning(std::string message) {
	LOG(WARNING) << message;
	
}

void GoogleLog::error(std::string message) {
	LOG(ERROR) << message;
}

//void GoogleLog::fatal(std::string message) {
//	//LOG(FATAL) << message;
//}