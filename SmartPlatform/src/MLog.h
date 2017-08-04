#ifndef mlog_h_
#define mlog_h_

#include "boost\utility.hpp"
#include "boost\shared_ptr.hpp"
#include <string>

class ILog;

class MLog : boost::noncopyable
{
public:
	enum LoggerType {
		Google = 1,
		Udp = 2,
	};

public:
	static MLog& instance();

public:
	MLog();
	//MLog(LoggerType logger_type);
	~MLog();

	void info(std::string message);
	void warning(std::string message);
	void error(std::string message);

private:
	static MLog *instance_;
	LoggerType logger_type_;
	boost::shared_ptr<ILog> log_;
};

#ifndef MLogger
#define MLogger MLog::instance()
#endif // !MLogInfo(message)


#endif // !mlog_h_


