#pragma once

#include "ServiceItem.h"
#include <string>
#include <vector>

class Config
{
public:
	Config(std::string path);
	~Config();

	std::vector<ServiceItem> get_service_items();

private:
	int load(std::string path);

private:
	std::vector<ServiceItem> service_items_;
};

