//#include "router.h"
#include "Util.h"
#include <iostream>
#include <string>
#include "cpp_redis/redis_client.hpp"
#include "cinatra_http/http_server.h"
#include <map>

#include <boost/asio.hpp>


using namespace cinatra;
using namespace boost;
using namespace std;



using handle_func = void(*)(cinatra::request const& req, cinatra::response& res);

std::map<std::string, handle_func> router_;

int test_redis();
std::string test_socket(std::string &request_data);

void camera_occlusion_service_request_handler(cinatra::request const& req, cinatra::response& res) {
	res.response_text("CameraOcclusionServiceRequest");
}

void request_handler(cinatra::request const& req, cinatra::response& res) {
	string path = req.path().to_string();
}

void init() {
	router_["/"] = [](cinatra::request const& req, cinatra::response& res) {
		res.response_text("Welcome");
	};

	router_["/CameraOcclusionServiceRequest"] = [](cinatra::request const& req, cinatra::response& res) {
		cout << "Request CameraOcclusionServiceRequest" << endl;
		res.add_header("Content-Type", "application/json");
		res.add_header("Access-Control-Allow-Origin", "*");
		string respons_data = test_socket(req.body().to_string());
		res.response_text(respons_data);
	};
}

int main()
{
	//return test_redis();


	init();

	cinatra::http_server s(4);
	s.request_handler([](cinatra::request const& req, cinatra::response& res)
	{
		string path = req.path().to_string();
		handle_func func = router_[path];
		if (func) {
			func(req, res);
		}
		else {
			res.response_text("Unimplemented");
		}
		
	});
	s.listen("0.0.0.0", "8080");
	s.run();

	return 0;
}


int test_redis() {
	cpp_redis::redis_client client;

	client.connect();

	client.set("hello", "42");
	client.get("hello", [](cpp_redis::reply& reply) {
		std::cout << reply << std::endl;
	});

	client.sync_commit();
	//# or client.commit(); for synchronous call

	return 0;
}

std::string test_socket(std::string &request_data){
	using namespace boost::asio;
	io_service iosev;
	ip::tcp::socket socket(iosev);
	ip::tcp::endpoint ep(ip::address_v4::from_string("192.168.0.88"), 8888);
	boost::system::error_code ec;
	socket.connect(ep, ec);
	if (ec) {
		std::cerr << boost::system::system_error(ec).what() << endl;
		return "";
	}


	unsigned char send_bytes[4] = { 0 };
	cout << "data_length=" << request_data.length() << endl;
	Util::int_to_byte(request_data.length(), send_bytes, 4);
	write(socket, buffer(send_bytes));

	cout << "data=" << request_data << endl;
	write(socket, buffer(request_data.c_str(), request_data.length()));

	unsigned char recv_bytes[4] = { 0 };
	read(socket, buffer(recv_bytes));
	int n_recv_bytes = Util::bytes_to_int(recv_bytes);
	cout << "service_data_length=" << n_recv_bytes << endl;

	char* recv_data = new char[n_recv_bytes];
	read(socket, buffer(recv_data, n_recv_bytes));
	string str_recv_data = string(recv_data, n_recv_bytes);
	cout << "service_data=" << str_recv_data << endl;

	//cout << recv_data << endl;

	delete recv_data;
	return str_recv_data;
}