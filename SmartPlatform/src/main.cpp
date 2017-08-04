#include "Common.h"
#include "Config.h"
#include "ImageSaveThread.h"
#include "MLog.h"
#include "Server.h"
#include "Session.h"
#include "Util.h"
#include <boost/asio.hpp>
#include "boost\thread\thread.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace boost;
using namespace std;
using boost::asio::ip::tcp;

int main(int argc, char** argv) {
	try {
		Config::instance()->init();
		boost::asio::io_service myIoService;
		short port = 8010;
		boost::asio::ip::tcp::endpoint endPoint(tcp::v4(), port);
		Server server(myIoService, endPoint);
		server.start();
		getchar();
		return 0;
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
	}
	catch (...) {
		cerr << "Unknown exception!" << endl;
	}
}
