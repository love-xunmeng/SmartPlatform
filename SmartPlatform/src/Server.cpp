#include "CigaretteAlgorithmThreadManager.h"
#include "Common.h"
#include "ImageSaveThread.h"
#include "MessageDispatcher.h"
#include "MLog.h"
#include "Server.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <sstream>
using boost::asio::ip::tcp;
using namespace std;


Server::Server(boost::asio::io_service& ioservice, tcp::endpoint& endpoint)
	:m_ioservice(ioservice),
	acceptor_(ioservice, endpoint)
{
	raw_message_data_queue_ = std::shared_ptr<RawMessageDataQueue>(new RawMessageDataQueue());
	session_ptr new_session(new Session(ioservice, raw_message_data_queue_));
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&Server::handle_accept, this, boost::asio::placeholders::error,
			new_session));
	dispatcher_ = nullptr;
}

Server::~Server()
{
}

void Server::start() {
	MLogger.info("Starting Server...");

	thread_ = boost::thread(boost::bind(&boost::asio::io_service::run, boost::ref(m_ioservice)));

	//
	const int cigarette_algorithm_thread_number = 1;

	ostringstream ss;
	ss << "CigaretteAlgorithmThread number is " << cigarette_algorithm_thread_number << endl;
	MLogger.info(ss.str());

	CigaretteAlgorithmThreadManager *cigarette_algorithm_thread_manager = new CigaretteAlgorithmThreadManager(cigarette_algorithm_thread_number);
	cigarette_algorithm_thread_manager->start();

	//
	dispatcher_ = std::shared_ptr<MessageDispatcher>(new MessageDispatcher(raw_message_data_queue_));
	dispatcher_->register_handler(SmartPlatform::CigaretteAlgorithm_Request, cigarette_algorithm_thread_manager);
	dispatcher_->start();

	ImageSaveThread::instance()->start();

	MLogger.info("Started Server. SmartPlatform is servicing...");
}

void Server::stop() {
	ImageSaveThread::instance()->stop();
	thread_.join();
}

void Server::handle_accept(const boost::system::error_code& error, session_ptr& session)
{
	if (!error)
	{
		std::cout << "get a new client!" << std::endl;
		session->start();

		session_ptr new_session(new Session(m_ioservice, raw_message_data_queue_));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&Server::handle_accept, this, boost::asio::placeholders::error,
				new_session));

		boost::asio::ip::tcp::no_delay noDelayOption(true);
		session->socket().set_option(noDelayOption);
	}
}
