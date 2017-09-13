#include "RequestHandler.h"
#include "CameraOcclusionServiceRequestHandler.h"
#include <fstream>
#include <sstream>
#include <string>

std::string process_route_request(void) {
	std::ifstream file("./data/test_interface.html");
	std::ostringstream ss;
	ss << file.rdbuf();
	std::string str = ss.str();
	return str;
}

crow::response process_camera_occlusion_service_request(const crow::request& req) {
	CameraOcclusionServiceRequestHandler handler;
	std::string body = req.body;
	std::string str = handler.process(body);
	if ("" == str) {
		str = "{\"Status\":501}";
	}
	crow::response resp{ str };
	resp.add_header("Access-Control-Allow-Origin", "*");
	return resp;
}

crow::response husband_wife_looks_request(const crow::request& req) {
	return crow::response("nothing");
}

//crow::response process_register_service_request(const crow::request& req) {
//	RegisterUnRegisterServiceHandler handler;
//	std::string body = req.body;
//	crow::response resp{ handler.register_service(body) };
//	resp.add_header("Access-Control-Allow-Origin", "*");
//	return resp;
//}
//
//crow::response process_unregister_occlusion_service_request(const crow::request& req) {
//	RegisterUnRegisterServiceHandler handler;
//	std::string body = req.body;
//	crow::response resp{ handler.unregister_service(body) };
//	resp.add_header("Access-Control-Allow-Origin", "*");
//	return resp;
//}