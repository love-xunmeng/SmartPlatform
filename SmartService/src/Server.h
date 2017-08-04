#ifndef server_h_
#define server_h_

#include "Session.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <memory>
#include <vector>

class MyThread2;
class QueueManager;

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
	void start_listerning();
	void start_algorithm_threads();
	void register_singal_handlers();
	void register_service_to_proxy();
	
private:
	boost::asio::io_service& m_ioservice;
	tcp::acceptor acceptor_;
	boost::thread asio_thread_;
	boost::shared_ptr<QueueManager> queue_manager_;
	std::vector<boost::shared_ptr<MyThread2>> algorithm_threads_;
};

#endif //server_h_

