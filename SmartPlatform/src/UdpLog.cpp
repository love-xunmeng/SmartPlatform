#include "UdpLog.h"

#include "boost\asio.hpp"
#include <string>

using namespace boost::asio;
using namespace std;

UdpLog::UdpLog()
	: remote_endpoint_(ip::address_v4::from_string("127.0.0.1"), 7777)
	,socket_(io_service_)
{
	socket_.open(remote_endpoint_.protocol());
}


UdpLog::~UdpLog()
{
}


void UdpLog::info(std::string message) {
	send("Info: " + message + "\n");
}

void UdpLog::warning(std::string message) {
	send("Warning: " + message + "\n");
}

void UdpLog::error(std::string message) {
	send("Error: " + message + "\n");
}

void UdpLog::send(std::string message) {
	boost::mutex::scoped_lock lock(mutex_socket_);
	socket_.send_to(buffer(message, message.length()), remote_endpoint_);
}
