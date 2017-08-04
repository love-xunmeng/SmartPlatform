#include "Config.h"
#include "Util.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
using namespace std;

Config* Config::instance_ = NULL;

Config::Config() {
	is_save_image_ =  false;
	//image_save_root_dir_ = "d:\\tmp\\IndustryCamera";
	//image_save_root_dir_ = "D:\\work\\data\\tt";
	image_save_root_dir_ = "d:\\tt";
}

Config::~Config() {

}

Config* Config::instance() {
	if (NULL == instance_) {
		instance_ = new Config();
	}
	return instance_;
}

int Config::init() {
	load_cigarette_map();
	return 0;
}

int Config::load_cigarette_map() {
	ifstream file(".\\data\\config\\cigarette_map.txt");
	if (!file.is_open()) {
		return -1;
	}

	const int buffer_size = 1024;
	char buffer[buffer_size] = { 0 };

	while (!file.eof()) {
		memset(buffer, 0, buffer_size);
		file.getline(buffer, buffer_size);
		if (0 == strlen(buffer)) {
			continue;
		}
		std::vector<std::string> splited_line = Util::split_by_stl(buffer, ",");
		CigaretteNode cigarette_node;
		cigarette_node.train_id_ = atoi(splited_line[0].c_str());
		cigarette_node.db_id_ = atoi(splited_line[1].c_str());
		cigarette_node.name_ = string(splited_line[2].c_str());

		cigarette_map_[cigarette_node.train_id_] = cigarette_node;
	}

	file.close();

	debug_print_all();

	return 0;
}

void Config::debug_print_all() {
	cout << "debug_print_all" << endl;
	std::map<int, CigaretteNode>::iterator iter = cigarette_map_.begin();
	while (iter != cigarette_map_.end()) {
		cout << iter->first << ", " << iter->second.db_id_ << ", " << iter->second.name_ << endl;
		++iter;
	}
	cout << "debug_print_all" << endl;
}

int Config::get_db_id_by_train_id(int train_id) {
	return cigarette_map_[train_id].db_id_;
}

bool Config::is_save_image() {
	return is_save_image_;
}