#ifndef server_h_
#define server_h_

#include "Common.h"
#include "Session.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <memory>

class MessageDispatcher;

class Server
{
public:
	Server(boost::asio::io_service& ioservice, tcp::endpoint& endpoint);
	~Server();

	void start();
	void stop();

	typedef boost::shared_ptr<Session> session_ptr;

private:
	void handle_accept(const boost::system::error_code& error, session_ptr& session);
	
private:
	boost::asio::io_service& m_ioservice;
	tcp::acceptor acceptor_;
	boost::thread thread_;
	RawMessageDataQueuePtr raw_message_data_queue_;
	std::shared_ptr<MessageDispatcher> dispatcher_;
};

#endif //server_h_

