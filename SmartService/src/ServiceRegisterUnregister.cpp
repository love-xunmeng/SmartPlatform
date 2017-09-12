#include "ServiceItem.h"
#include "ServiceRegisterUnregister.h"
#include "cpp_redis/cpp_redis"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

ServiceRegisterUnregister::ServiceRegisterUnregister(std::vector<ServiceItem> service_items)
	:service_items_(service_items)
{
}


ServiceRegisterUnregister::~ServiceRegisterUnregister()
{
}

int ServiceRegisterUnregister::register_service() {
	cpp_redis::redis_client client;
	client.connect("127.10.0.1", 6379, [](cpp_redis::redis_client&) {
		std::cout << "client disconnected (disconnection handler)" << std::endl;
	});
	for each(auto item in service_items_) {
		std::vector<string> values;
		ostringstream ss;
		ss << item.host_ << ":" << item.port_;
		values.push_back(ss.str());
		client.sadd(item.algorithm_name_, values);
	}
	client.sync_commit();
	return 0;
}

int ServiceRegisterUnregister::unregister_service() {
	cpp_redis::redis_client client;
	client.connect("127.10.0.1", 6379, [](cpp_redis::redis_client&) {
		std::cout << "client disconnected (disconnection handler)" << std::endl;
	});
	for each(auto item in service_items_) {
		std::vector<string> values;
		ostringstream ss;
		ss << item.host_ << ":" << item.port_;
		values.push_back(ss.str());
		client.srem(item.algorithm_name_, values);
	}
	client.sync_commit();
	return 0;
}
