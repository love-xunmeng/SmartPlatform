#pragma once

#include <string>

class CameraOcclusionServiceRequestHandler
{
public:
	CameraOcclusionServiceRequestHandler();
	~CameraOcclusionServiceRequestHandler();

	std::string process(std::string &request_data);
};

