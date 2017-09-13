//#include "MySqlRunnable.h"
//#include "MySqlConnectionPool.h"
//#include "mysql_connect.h"
#include "RequestHandler.h"
#include "ServiceUpdater.h"
#include "crow.h"
#include "boost/asio.hpp"
#include "boost/make_shared.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"
#include <iostream>
using namespace boost;
using namespace std;

//int test_mysql_connection_pool();
//int test_mysql_query(boost::shared_ptr<MySqlRunnable> mysql_connection);
//int test_mysql_insert(boost::shared_ptr<MySqlRunnable> mysql_connection);

int main()
{
	//boost::shared_ptr<MySqlConnectionPool> mysql_connection_pool = boost::make_shared<MySqlConnectionPool>(std::thread::hardware_concurrency());

	boost::asio::io_service ios;
	ServiceUpdater::instance()->set(ios, 500, true);
	ServiceUpdater::instance()->start();
	boost::thread asio_thread_ = boost::thread(boost::bind(&boost::asio::io_service::run, boost::ref(ios)));
	
	crow::SimpleApp app;
	app.route_dynamic("/").methods(crow::HTTPMethod::Post)([] {return process_route_request();});
	app.route_dynamic("/CameraOcclusionServiceRequest").methods(crow::HTTPMethod::Post)([](const crow::request& req) {return process_camera_occlusion_service_request(req); });
	app.route_dynamic("/HusbandWithLooksRequest").methods(crow::HTTPMethod::Post)([](const crow::request& req) {return husband_wife_looks_request(req); });
	
	//CROW_ROUTE(app, "/")(process_route_request);
	///CROW_ROUTE(app, "/CameraOcclusionServiceRequest").methods("POST"_method)(process_camera_occlusion_service_request);
	
	
	//CROW_ROUTE(app, "/TestMySqlInsert")([&] {
	//	boost::shared_ptr<MySqlRunnable> mysql_connection = mysql_connection_pool->get_connection();
	//	test_mysql_insert(mysql_connection); 
	//	mysql_connection_pool->return_connection(mysql_connection);
	//	return "Success"; });
	//CROW_ROUTE(app, "/TestMySqlQuery")([&] {
	//	boost::shared_ptr<MySqlRunnable> mysql_connection = mysql_connection_pool->get_connection();
	//	test_mysql_query(mysql_connection);
	//	if (nullptr != mysql_connection) {
	//		mysql_connection_pool->return_connection(mysql_connection);
	//	}
	//	return "Success"; });
	
	crow::logger::setLogLevel(crow::LogLevel::Error);
	app.port(18080).multithreaded().run();

	getchar();
	ServiceUpdater::instance()->stop();
}

//int test_mysql_connection_pool() {
//	ConnectionPool *pool = ConnectionPool::getInstance();
//	std::shared_ptr<Connection>con;
//	Statement *state;
//	ResultSet *result;
//
//	//获得一个连接
//	con = pool->getConnect();
//	//获得一个数据库连接对象
//	state = con->createStatement();
//	//使用XL_db这个数据库
//	state->execute("use XL_db");
//	//查询语句
//	result = state->executeQuery("select * from UserInfo;");
//	while (result->next())
//	{
//		int id = result->getInt("uid");
//		std::string name = result->getString("password");
//		std::cout << "id:" << id << " name:" << name << std::endl;
//	}
//
//	boost::this_thread::sleep(boost::posix_time::seconds(10));
//	pool->retConnect(con);
//	std::cout << pool->getPoolSize() << std::endl;
//	boost::this_thread::sleep(boost::posix_time::seconds(10));
//
//	return EXIT_SUCCESS;
//}
//
//int test_mysql_query(boost::shared_ptr<MySqlRunnable> mysql_connection) {
//	mysql_connection->execute_query("select * from user");
//	return 0;
//}
//
//int test_mysql_insert(boost::shared_ptr<MySqlRunnable> mysql_connection) {
//	//mysql_connection->insert();
//	return 0;
//}