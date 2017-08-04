#pragma once

#include "IAlgorithm.h"
#include "boost/shared_ptr.hpp"

class CameraAbnormalDetector;

class CameraObscureAlgorithm : public IAlgorithm
{
public:
	CameraObscureAlgorithm();
	~CameraObscureAlgorithm();

	virtual std::string process(std::string data);

private:
	std::string process_server_error();

private:
	boost::shared_ptr<CameraAbnormalDetector> algorithm_;
};

