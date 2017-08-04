#pragma once
#include <string>
class RegisterUnRegisterServiceHandler
{
public:
	RegisterUnRegisterServiceHandler();
	~RegisterUnRegisterServiceHandler();

	std::string register_service(std::string &request_data);
	std::string unregister_service(std::string &request_data);
};

