#include "Server.h"
#include "Algorithm.h"
#include "CameraObscureAlgorithm.h"
#include "AlgorithmThread.h"
#include "QueueManager.h"
#include "MyThread2.h"
#include "Session.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <sstream>
#include <vector>

using boost::asio::ip::tcp;
using namespace std;

#ifdef linux
BOOL WINAPI CtrlHandler(DWORD dwCtrlType);
#endif

Server::Server(boost::asio::io_service& ioservice, tcp::endpoint& endpoint)
	: m_ioservice(ioservice)
	, acceptor_(ioservice, endpoint)
	, queue_manager_(new QueueManager())
{
	session_ptr new_session(new Session(ioservice, queue_manager_->get_queue()));
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&Server::handle_accept, this, boost::asio::placeholders::error,
			new_session));
}

Server::~Server()
{
}

void Server::start() {
	start_listerning();
	start_algorithm_threads();
	register_singal_handlers();
	register_service_to_proxy();
}

void Server::stop() {
	asio_thread_.join();
	for each(boost::shared_ptr<MyThread2> thread in algorithm_threads_) {
		thread->stop();
	}
}

void Server::handle_accept(const boost::system::error_code& error, session_ptr& session)
{
	if (!error)
	{
		std::cout << "get a new client!" << std::endl;
		session->start();

		session_ptr new_session(new Session(m_ioservice, queue_manager_->get_queue()));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&Server::handle_accept, this, boost::asio::placeholders::error,
				new_session));

		boost::asio::ip::tcp::no_delay noDelayOption(true);
		session->socket().set_option(noDelayOption);
	}
}

void Server::start_listerning() {
	asio_thread_ = boost::thread(boost::bind(&boost::asio::io_service::run, boost::ref(m_ioservice)));
}

void Server::start_algorithm_threads() {
	for (int i = 0; i < 4; i++) {
		boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>> queue = queue_manager_->get_queue();
		boost::shared_ptr<MyThread2> thread = boost::shared_ptr<MyThread2>(new MyThread2(queue,
			boost::shared_ptr<IAlgorithm>(new CameraObscureAlgorithm())));
		thread->start();
		algorithm_threads_.push_back(thread);
	}
}

void Server::register_singal_handlers() {
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
}

void Server::register_service_to_proxy() {

}

#ifdef linux
BOOL WINAPI CtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		//getchar();
		return(TRUE);

	case CTRL_CLOSE_EVENT:
		printf("Ctrl-Close event\n\n");;
		return(TRUE);

	case CTRL_BREAK_EVENT:
		printf("Ctrl-Break event\n\n");
		return FALSE; // pass thru, let the system to handle the event.

	case CTRL_LOGOFF_EVENT:
		printf("Ctrl-Logoff event\n\n");
		return FALSE; // pass thru, let the system to handle the event.

	case CTRL_SHUTDOWN_EVENT:
		printf("Ctrl-Shutdown event\n\n");
		return FALSE; // pass thru, let the system to handle the event.

	default:
		return FALSE;
	}
	return TRUE;
}
#endif