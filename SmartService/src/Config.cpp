#include "Config.h"
#include "ServiceItem.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include <iostream>
#include <string>

using namespace boost::property_tree;
using namespace boost;
using namespace std;

Config::Config(std::string path)
{
	load(path);
}


Config::~Config()
{
}

int Config::load(std::string path) {
	service_items_.clear();
	ptree pt;
	read_xml(path, pt);
	ptree child = pt.get_child("Service");
	for (auto pos = child.begin(); pos != child.end(); ++pos) {
		ServiceItem service_item;
		service_item.algorithm_name_ = pos->second.get<string>("AlgorithmName");
		service_item.host_ = pos->second.get<string>("Host");
		service_item.port_ = pos->second.get<int>("Port");
		service_items_.push_back(service_item);
	}
	return 0;
}

std::vector<ServiceItem> Config::get_service_items() {
	return service_items_;
}
