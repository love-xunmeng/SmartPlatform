#ifndef google_log_h_
#define google_log_h_

#include "ILog.h"
#include <string>

class GoogleLog : public ILog
{
public:
	GoogleLog();
	~GoogleLog();

	virtual void info(std::string message);
	virtual void warning(std::string message);
	virtual void error(std::string message);
};

#endif // !google_log_h_


