#include "RegisterUnRegisterServiceHandler.h"
#include "rapidjson/document.h"     
#include "rapidjson/prettywriter.h" 
#include "cpp_redis/cpp_redis"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace rapidjson;
using namespace std;

RegisterUnRegisterServiceHandler::RegisterUnRegisterServiceHandler()
{
}


RegisterUnRegisterServiceHandler::~RegisterUnRegisterServiceHandler()
{
}

std::string RegisterUnRegisterServiceHandler::register_service(std::string &request_data) {
	request_data = "{\"items\": [{\"algorithm_name\": \"camera\",\"host\" : \"192.168.0.88\",\"port\" : 8088}, \
						{\"algorithm_name\": \"face_detect\",\"host\" : \"192.168.0.88\",\"port\" : 8089}]}";

	Document document;
	document.Parse(request_data.c_str());

	Value &items = document["items"];
	if (!items.IsArray()) {
		cerr << request_data << " is not array" << endl;
		return "Error";
	}

	cpp_redis::redis_client client;
	client.connect("127.10.0.1", 6379, [](cpp_redis::redis_client&) {
		std::cout << "client disconnected (disconnection handler)" << std::endl;
	});

	for (size_t i = 0; i < items.Size(); ++i) {
		std::vector<string> values;
		Value &v = items[i];
		string algorithm_name = v["algorithm_name"].GetString();
		string host = v["host"].GetString();
		int port = v["port"].GetInt();		
		ostringstream ss;
		ss << host << ":" << port;
		values.push_back(ss.str());
		client.sadd(algorithm_name, values);
	}
	client.sync_commit();

	return "RegisterServie OK";
}

std::string RegisterUnRegisterServiceHandler::unregister_service(std::string &request_data) {
	request_data = "{\"items\": [{\"algorithm_name\": \"camera\",\"host\" : \"192.168.0.88\",\"port\" : 8088}, \
						{\"algorithm_name\": \"face_detect\",\"host\" : \"192.168.0.88\",\"port\" : 8089}]}";

	Document document;
	document.Parse(request_data.c_str());

	Value &items = document["items"];
	if (!items.IsArray()) {
		cerr << request_data << " is not array" << endl;
		return "Error";
	}

	cpp_redis::redis_client client;
	client.connect("127.10.0.1", 6379, [](cpp_redis::redis_client&) {
		std::cout << "client disconnected (disconnection handler)" << std::endl;
	});

	for (size_t i = 0; i < items.Size(); ++i) {
		std::vector<string> values;
		Value &v = items[i];
		string algorithm_name = v["algorithm_name"].GetString();
		string host = v["host"].GetString();
		int port = v["port"].GetInt();
		ostringstream ss;
		ss << host << ":" << port;
		values.push_back(ss.str());
		client.srem(algorithm_name, values);
	}
	client.sync_commit();

	return "UnRegisterServie OK";
}