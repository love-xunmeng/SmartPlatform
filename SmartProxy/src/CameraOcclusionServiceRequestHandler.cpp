#include "CameraOcclusionServiceRequestHandler.h"
#include "ServiceItem.h"
#include "ServiceUpdater.h"
#include "Util.h"
#include "boost/asio.hpp"
#include <iostream>

using namespace boost;
using namespace std;

CameraOcclusionServiceRequestHandler::CameraOcclusionServiceRequestHandler()
{
}


CameraOcclusionServiceRequestHandler::~CameraOcclusionServiceRequestHandler()
{
}

std::string CameraOcclusionServiceRequestHandler::process(std::string &request_data) {
	ServiceItem service_item = ServiceUpdater::instance()->get_camera_service_item();
	if ("" == service_item.host_ || 0 == service_item.port_) {
		return "";
	}

	using namespace boost::asio;
	io_service iosev;
	ip::tcp::socket socket(iosev);
	ip::tcp::endpoint ep(ip::address_v4::from_string(service_item.host_), service_item.port_);
	boost::system::error_code ec;
	socket.connect(ep, ec);
	if (ec) {
		std::cerr << boost::system::system_error(ec).what() << endl;
		return "";
	}

	unsigned char send_bytes[4] = { 0 };
	//cout << "data_length=" << request_data.length() << endl;
	Util::int_to_byte(request_data.length(), send_bytes, 4);
	write(socket, buffer(send_bytes));

	write(socket, buffer(request_data.c_str(), request_data.length()));

	unsigned char recv_bytes[4] = { 0 };
	read(socket, buffer(recv_bytes));
	int n_recv_bytes = Util::bytes_to_int(recv_bytes);
	//cout << "service_data_length=" << n_recv_bytes << endl;

	char* recv_data = new char[n_recv_bytes];
	read(socket, buffer(recv_data, n_recv_bytes));
	string str_recv_data = string(recv_data, n_recv_bytes);
	//cout << "service_data=" << str_recv_data << endl;

	delete recv_data;
	return str_recv_data;
}
