#include "RequestHandler.h"
#include "crow.h"
#include <iostream>
using namespace std;


int main()
{
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")(process_route_request);
	CROW_ROUTE(app, "/CameraOcclusionServiceRequest").methods("POST"_method)(process_camera_occlusion_service_request);
	CROW_ROUTE(app, "/RegisterService").methods("POST"_method)(process_register_service_request);
	CROW_ROUTE(app, "/UnRegisterService").methods("POST"_method)(process_unregister_occlusion_service_request);

	app.port(18080).multithreaded().run();
}