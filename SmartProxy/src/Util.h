#pragma once

#include <string>
#include <vector>

class Util
{
public:
	Util(void);
	~Util(void);

	static int get_dirs(const std::string &dir, std::vector<std::string> &dir_set);
	static size_t get_filenames(const std::string &dir, std::vector<std::string> &filenames, const std::string &pattern);
	static std::vector<std::string> split_by_stl(const std::string &target, const std::string &pattern);
	static std::vector<std::string> split_by_boost(const std::string &str, const std::string &patter);
	static void get_last_dir_name(const std::vector<std::string> &full_path_set, std::vector<std::string> &last_dir_name_set);
	static int create_dir_set(const std::string &root, const std::vector<std::string> &dir_set);
	static void move_file(const std::string &src, const std::string &dst);
	static void  int_to_byte(int i, unsigned char *bytes, int size = 4);
	static int bytes_to_int(unsigned char* bytes, int size = 4);
};

