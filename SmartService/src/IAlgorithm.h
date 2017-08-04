#pragma once

#include <string>

class IAlgorithm {
public:
	virtual std::string process(std::string data) = 0;
};