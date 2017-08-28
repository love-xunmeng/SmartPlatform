#include "Server.h"
#include "DataItem.h"
#include "MyThread2.h"
#include "HttpClient.h"
#include "boost/asio.hpp"
#include <iostream>
using namespace std;



int main(int argc, char* argv[]) {
	try {
		boost::asio::io_service myIoService;
		short port = 8888;
		boost::asio::ip::tcp::endpoint endPoint(tcp::v4(), port);
		Server server(myIoService, endPoint);
		server.start();
		getchar();
		server.stop();
		return 0;
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
	}
	catch (...) {
		cerr << "Unknown exception!" << endl;
	}
}