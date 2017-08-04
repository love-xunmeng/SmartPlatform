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

	vector<pair<string, string>> field_value;
	for (size_t i = 0; i < items.Size(); ++i) {
		field_value.clear();
		Value &v = items[i];
		string algorithm_name = v["algorithm_name"].GetString();
		string host = v["host"].GetString();
		int port = v["port"].GetInt();		
		field_value.push_back(std::make_pair("host", host));
		ostringstream ss;
		ss << port;
		field_value.push_back(std::make_pair("port", ss.str()));
		client.hmset(algorithm_name, field_value);
	}
	client.sync_commit();

	return "RegisterServie OK";
}

std::string RegisterUnRegisterServiceHandler::unregister_service(std::string &request_data) {
	return "UnRegisterServie OK";
}