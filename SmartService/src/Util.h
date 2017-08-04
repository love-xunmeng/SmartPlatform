#pragma once

#include <string>

class Util
{
public:
	Util();
	~Util();

	static bool Base64Encode(const std::string& input, std::string* output);
	static bool Base64Decode(const std::string& input, std::string* output);
	static void  int_to_byte(int i, unsigned char *bytes, int size = 4);
	static int bytes_to_int(unsigned char* bytes, int size = 4);
};

