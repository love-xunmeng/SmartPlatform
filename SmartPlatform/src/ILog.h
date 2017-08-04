#ifndef ilog_h_
#define ilog_h_

#include <string>

class ILog
{
public:
	ILog();
	~ILog();

	virtual void info(std::string message) = 0;
	virtual void warning(std::string message) = 0;
	virtual void error(std::string message) = 0;
};


#endif // !ilog_h_
