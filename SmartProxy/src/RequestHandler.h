#pragma once

#include "crow.h"
#include <string>

std::string process_route_request(void);
crow::response process_camera_occlusion_service_request(const crow::request& req);
crow::response process_register_service_request(const crow::request& req);
crow::response process_unregister_occlusion_service_request(const crow::request& req);

