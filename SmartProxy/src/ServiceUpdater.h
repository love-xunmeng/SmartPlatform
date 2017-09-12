#pragma once

#include "ServiceItem.h"
#include "boost/asio.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"
#include "boost/utility.hpp"

class ServiceUpdater{
public:
	~ServiceUpdater();
	void set(boost::asio::io_service &ios, int interval, bool start = true);
	static ServiceUpdater* instance();

	void start();
	void update(const boost::system::error_code &error_code);
	ServiceItem get_camera_service_item();
	ServiceItem get_face_detect_service_item();
	void stop();

private:
	ServiceUpdater();

private:
	static ServiceUpdater *instance_;
	bool is_run_;
	boost::function<void()> func_;
	int interval_;
	boost::shared_ptr<boost::asio::deadline_timer> timer_;
	int camera_service_item_index_;
	std::vector<ServiceItem> camera_service_items_;
	int face_detect_service_item_index_;
	std::vector<ServiceItem> face_detect_service_items_;
};