#ifndef udp_log_h_
#define udp_log_h_

#include "ILog.h"
#include "boost/asio.hpp"
#include "boost/thread/mutex.hpp"
#include <string>

class UdpLog : public ILog
{
public:
	UdpLog();
	~UdpLog();

	virtual void info(std::string message);
	virtual void warning(std::string message);
	virtual void error(std::string message);
	
private:
	void send(std::string message);

private:
	boost::asio::io_service io_service_;
	boost::asio::ip::udp::endpoint remote_endpoint_;
	boost::asio::ip::udp::socket socket_;
	boost::mutex mutex_socket_;
};

#endif // !udp_log_h_



