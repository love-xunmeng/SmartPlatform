#pragma once

#include <string>

class HttpClient
{
public:
	HttpClient();
	~HttpClient();

	int request(std::string host, int port, std::string request_path, std::string data);
};

