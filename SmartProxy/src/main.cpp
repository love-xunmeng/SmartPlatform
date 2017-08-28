#include "RequestHandler.h"
#include "crow.h"
#include <iostream>
using namespace std;


int main()
{
	crow::SimpleApp app;

	app.route_dynamic("/")([] {
		return "Hello World!";
	});

	//app.route_dynamic("/")(&process_route_request);

	//CROW_ROUTE(app, "/")(process_route_request);
	//CROW_ROUTE(app, "/CameraOcclusionServiceRequest").methods(crow::HTTPMethod::Post)(process_camera_occlusion_service_request);
	//CROW_ROUTE(app, "/RegisterService").methods(crow::HTTPMethod::Post)(process_register_service_request);
	//CROW_ROUTE(app, "/UnRegisterService").methods(crow::HTTPMethod::Post)(process_unregister_occlusion_service_request);

	app.port(18080).multithreaded().run();
}