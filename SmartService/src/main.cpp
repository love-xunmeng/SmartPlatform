#include "Config.h"
#include "Server.h"
#include "DataItem.h"
#include "MyThread.h"
#include "rapidjson/document.h"   
#include "boost/asio.hpp"
#include <iostream>
using namespace std;

static boost::shared_ptr<Server> server = nullptr;
BOOL WINAPI CtrlHandler(DWORD dwCtrlType);

int main(int argc, char* argv[]) {
	try {
		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
		boost::shared_ptr<Config> config(new Config("./data/config/service_config.xml"));
		boost::asio::io_service myIoService;
		short port = 8888;
		boost::asio::ip::tcp::endpoint endPoint(tcp::v4(), port);
		server = boost::shared_ptr<Server>(new Server(myIoService, endPoint, config));
		server->start();
		getchar();
		server->stop();
		return 0;
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
	}
	catch (...) {
		cerr << "Unknown exception!" << endl;
	}
}

BOOL WINAPI CtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		server->stop();
		return FALSE;
	default:
		return FALSE;
	}
	return TRUE;
}