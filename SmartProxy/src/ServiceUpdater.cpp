#include "ServiceUpdater.h"
#include "ServiceItem.h"
#include "Util.h"
#include "cpp_redis/cpp_redis"
#include <iostream>
#include <vector>
using namespace std;

ServiceUpdater* ServiceUpdater::instance_ = nullptr;

ServiceUpdater::ServiceUpdater(){
	camera_service_item_index_ = 0;
	face_detect_service_item_index_ = 0;
}

ServiceUpdater::~ServiceUpdater() {

}

ServiceUpdater* ServiceUpdater::instance() {
	if (nullptr == instance_) {
		instance_ = new ServiceUpdater();
	}
	return instance_;
}

void ServiceUpdater::set(boost::asio::io_service &ios, int interval, bool start) {
	interval_ = interval;
	timer_ = boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(ios, boost::posix_time::microsec(interval)));
	is_run_ = start;
}

void ServiceUpdater::start() {
	is_run_ = true;
	timer_->expires_from_now(boost::posix_time::millisec(interval_));
	timer_->async_wait(boost::bind(&ServiceUpdater::update, this, boost::asio::placeholders::error));
}

void ServiceUpdater::update(const boost::system::error_code &error_code) {
	if (!is_run_) {
		return;
	}

	cpp_redis::redis_client client;

	try {	
		client.connect("127.0.0.1", 6379, [](cpp_redis::redis_client&) {
			std::cout << "client disconnected (disconnection handler)" << std::endl;
		});
	}
	catch(std::exception &e){
		cerr << "Can NOT connect to redis 127.0.0.1:6379" << endl;
		timer_->expires_at(timer_->expires_at() + boost::posix_time::millisec(interval_));
		timer_->async_wait(boost::bind(&ServiceUpdater::update, this, boost::asio::placeholders::error));
		return;
	}

	client.smembers("camera", [this](cpp_redis::reply &reply) {
		vector<ServiceItem> camera_service_items;
		const vector<cpp_redis::reply> reply_array = reply.as_array();
		for (int i = 0; i < reply_array.size(); i++) {
			string str_host_port = reply_array[i].as_string();
			vector<string> splited_item = Util::split_by_stl(str_host_port, ":");
			ServiceItem service_item;
			service_item.algorithm_name_ = "camera";
			service_item.host_ = splited_item[0];
			service_item.port_ = atoi(splited_item[1].c_str());
			camera_service_items.push_back(service_item);
		}
		this->camera_service_items_ = camera_service_items;
	});

	vector<ServiceItem> face_detect_service_items;
	client.smembers("face_detect", [&face_detect_service_items](cpp_redis::reply &reply) {
		const vector<cpp_redis::reply> replay_array = reply.as_array();
		for (int i = 0; i < replay_array.size(); i++) {
			string str_host_port = replay_array[i].as_string();
			vector<string> splited_item = Util::split_by_stl(str_host_port, ":");
			ServiceItem service_item;
			service_item.algorithm_name_ = "face_detect";
			service_item.host_ = splited_item[0];
			service_item.port_ = atoi(splited_item[1].c_str());
			face_detect_service_items.push_back(service_item);
		}
	});
	face_detect_service_items_ = face_detect_service_items;

	client.sync_commit();

	timer_->expires_at(timer_->expires_at() + boost::posix_time::millisec(interval_));
	timer_->async_wait(boost::bind(&ServiceUpdater::update, this, boost::asio::placeholders::error));
}

ServiceItem ServiceUpdater::get_camera_service_item() {
	if (0 == camera_service_items_.size()) {
		return ServiceItem();
	}
	ServiceItem service_item = camera_service_items_[camera_service_item_index_];
	camera_service_item_index_ = ++camera_service_item_index_ % camera_service_items_.size();
	return service_item;
}

ServiceItem ServiceUpdater::get_face_detect_service_item() {
	if (0 == face_detect_service_items_.size()) {
		return ServiceItem();
	}
	ServiceItem service_item = face_detect_service_items_[face_detect_service_item_index_];
	face_detect_service_item_index_ = ++face_detect_service_item_index_ % face_detect_service_items_.size();
	return service_item;
}

void ServiceUpdater::stop() {
	is_run_ = false;
}