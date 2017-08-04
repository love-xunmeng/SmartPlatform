#pragma once

#include <string>

class Algorithm {
public:
	Algorithm() {

	}

	~Algorithm() {

	}

	virtual int process(std::string data) {
		return 0;
	}
};