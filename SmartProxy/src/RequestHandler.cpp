#include "RequestHandler.h"
#include "CameraOcclusionServiceRequestHandler.h"
#include "RegisterUnRegisterServiceHandler.h"

std::string process_route_request(void) {
	return "Hello, Welcome to SmartPlatform!";
}

crow::response process_camera_occlusion_service_request(const crow::request& req) {
	CameraOcclusionServiceRequestHandler handler;
	std::string body = req.body;
	return crow::response{ handler.process(body) };
}

crow::response process_register_service_request(const crow::request& req) {
	RegisterUnRegisterServiceHandler handler;
	std::string body = req.body;
	crow::response resp{ handler.register_service(body) };
	resp.add_header("Access-Control-Allow-Origin", "*");
	return resp;
}

crow::response process_unregister_occlusion_service_request(const crow::request& req) {
	RegisterUnRegisterServiceHandler handler;
	std::string body = req.body;
	crow::response resp{ handler.register_service(body) };
	resp.add_header("Access-Control-Allow-Origin", "*");
	return resp;
}