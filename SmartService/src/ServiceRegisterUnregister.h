#pragma once

#include "ServiceItem.h"
#include <string>
#include <vector>

class ServiceRegisterUnregister
{
public:
	ServiceRegisterUnregister(std::vector<ServiceItem> service_items);
	~ServiceRegisterUnregister();

	int register_service();
	int unregister_service();

private:
	std::vector<ServiceItem> service_items_;
};

