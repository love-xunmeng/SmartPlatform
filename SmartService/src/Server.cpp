#include "Server.h"
#include "CameraObscureAlgorithm.h"
#include "Config.h"
#include "QueueManager.h"
#include "MyThread.h"
#include "Session.h"
#include "ServiceRegisterUnregister.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <sstream>
#include <vector>

using boost::asio::ip::tcp;
using namespace std;



Server::Server(boost::asio::io_service& ioservice, tcp::endpoint& endpoint, boost::shared_ptr<Config> config)
	: m_ioservice(ioservice)
	, acceptor_(ioservice, endpoint)
	, queue_manager_(new QueueManager())
	, config_(config)
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
	register_service();
}

void Server::stop() {
	m_ioservice.stop();
	asio_thread_.join();
	for each(boost::shared_ptr<MyThread> thread in algorithm_threads_) {
		thread->stop();
	}
	unregister_service();
}

void Server::handle_accept(const boost::system::error_code& error, session_ptr& session)
{
	if (!error)
	{
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
		boost::shared_ptr<MyThread> thread = boost::shared_ptr<MyThread>(new MyThread(queue,
			boost::shared_ptr<IAlgorithm>(new CameraObscureAlgorithm())));
		thread->start();
		algorithm_threads_.push_back(thread);
	}
}

void Server::register_service() {
	ServiceRegisterUnregister service(config_->get_service_items());
	service.register_service();
}

void Server::unregister_service() {
	ServiceRegisterUnregister service(config_->get_service_items());
	service.unregister_service();
}