#pragma once

#include "boost/shared_ptr.hpp"
#include <string>

class Session;

class DataItem {
public:
	DataItem() {

	}

	~DataItem() {

	}

public:
	std::string json_data_ = "";
	boost::shared_ptr<Session> session_ = nullptr;
};