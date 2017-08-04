#include "Util.h"
#include <cassert>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <direct.h>

using namespace std;
namespace fs = boost::filesystem;

Util::Util(void)
{
}


Util::~Util(void)
{
}

int Util::get_dirs(const std::string &dir, std::vector<std::string> &dir_set) {
	fs::path path(dir);
	if (!fs::exists(path)) {
		return -1;
	}

	fs::directory_iterator end_iter;
	for (fs::directory_iterator iter(path); iter != end_iter; ++iter) {
		if (fs::is_directory(iter->status())) {
			dir_set.push_back(iter->path().string());
		}
	}

	return 0;
}

size_t Util::get_filenames(const std::string &dir, std::vector<std::string> &filenames, const std::string &pattern){
	fs::path path(dir);
	if(!fs::exists(path)){
		return -1;
	}

	fs::directory_iterator end_iter;
	for(fs::directory_iterator iter(path); iter != end_iter; ++iter){
		if(fs::is_regular_file(iter->status())){
			if(fs::extension(*iter) == pattern){
				filenames.push_back(iter->path().string());
			}
			
		}
		if(fs::is_directory(iter->status())){
			get_filenames(iter->path().string(), filenames, pattern);
		}
	}

	return filenames.size();
}

std::vector<std::string> Util::split_by_stl(const std::string &target, const std::string &pattern) {
	std::string::size_type pos;
	std::vector<std::string> result;
	std::string str = target + pattern;
	size_t size = str.size();

	for (size_t i = 0; i < size; ++i) {
		pos = str.find(pattern, i);
		if (pos < size) {
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	
	return result;
}

void Util::get_last_dir_name(const std::vector<std::string> &full_path_set, std::vector<std::string> &last_dir_name_set) {
	const std::string pattern = "\\";
	for (int i = 0; i < full_path_set.size(); ++i) {
		std::string full_path = full_path_set[i];
		std::vector<std::string> splited_path = Util::split_by_stl(full_path, pattern);
		last_dir_name_set.push_back(splited_path[splited_path.size() - 1]);
	}
}

int Util::create_dir_set(const ::string &root, const std::vector<std::string> &dir_set) {
	for (int i = 0; i < dir_set.size(); ++i) {
		std::string full_path = root + "\\" + dir_set[i] + "\\face";
		//_mkdir(full_path.c_str());
		boost::filesystem::create_directories(full_path);
	}

	return 0;
}

void Util::move_file(const std::string &src, const std::string &dst) {
	boost::filesystem::copy_file(src, dst);
	boost::filesystem::remove(src);
}

void  Util::int_to_byte(int i, unsigned char *bytes, int size){
	memset(bytes, 0, sizeof(unsigned char) *  size);
	bytes[0] = (unsigned char)(0xff & i);
	bytes[1] = (unsigned char)((0xff00 & i) >> 8);
	bytes[2] = (unsigned char)((0xff0000 & i) >> 16);
	bytes[3] = (unsigned char)((0xff000000 & i) >> 24);
	return;
}

int Util::bytes_to_int(unsigned char* bytes, int size){
	int addr = bytes[0] & 0xFF;
	addr |= ((bytes[1] << 8) & 0xFF00);
	addr |= ((bytes[2] << 16) & 0xFF0000);
	addr |= ((bytes[3] << 24) & 0xFF000000);
	return addr;
}