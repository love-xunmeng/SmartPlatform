#include "Config.h"
#include "Server.h"
#include "DataItem.h"
#include "MyThread.h"
#include "TestFaceDetection.inc.h"
#include "rapidjson/document.h"   
#include "opencv2/opencv.hpp"
#include "boost/asio.hpp"
#include <chrono>
#include <iostream>
using namespace std;


static boost::shared_ptr<Server> server = nullptr;
BOOL WINAPI CtrlHandler(DWORD dwCtrlType);

caffe::CaffeBinding* kCaffeBinding = new caffe::CaffeBinding();
int test_mtcnn();

int main(int argc, char* argv[]) {
	return test_mtcnn();

	try {
		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
		boost::shared_ptr<Config> config(new Config("./data/config/service_config.xml"));
		boost::asio::io_service myIoService;
		short port = 8888;
		boost::asio::ip::tcp::endpoint endPoint(tcp::v4(), port);
		server = boost::shared_ptr<Server>(new Server(myIoService, endPoint, config));
		server->start();
		getchar();
		server->stop();
		return 0;
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
	}
	catch (...) {
		cerr << "Unknown exception!" << endl;
	}
}

BOOL WINAPI CtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		server->stop();
		return FALSE;
	default:
		return FALSE;
	}
	return TRUE;
}

int test_mtcnn() {
	using namespace FaceInception;
	using namespace cv;

	string model_folder = ".\\data\\model\\MTCNNv2\\model\\";
	CascadeCNN cascade(model_folder + "det1-memory.prototxt", model_folder + "det1.caffemodel",
		model_folder + "det1-memory-stitch.prototxt", model_folder + "det1.caffemodel",
		model_folder + "det2-memory.prototxt", model_folder + "det2.caffemodel",
		model_folder + "det3-memory.prototxt", model_folder + "det3.caffemodel",
		model_folder + "det4-memory.prototxt", model_folder + "det4.caffemodel",
		-1);

	Mat image = imread("e:\\jingtian.jpg");

	double min_face_size = image.cols / 4;
	cout << min_face_size << endl;

	cout << image.cols << "," << image.rows << endl;
	vector<vector<Point2d>> points;
	std::chrono::time_point<std::chrono::system_clock> p0 = std::chrono::system_clock::now();
	auto result = cascade.GetDetection(image, 12.0 / min_face_size, 0.7, true, 0.7, true, points);
	std::chrono::time_point<std::chrono::system_clock> p1 = std::chrono::system_clock::now();
	cout << "detection time:" << (float)std::chrono::duration_cast<std::chrono::microseconds>(p1 - p0).count() / 1000 << "ms" << endl;


	points.clear();//The first run is slow because it need to allocate memory.
	p0 = std::chrono::system_clock::now();
	cout << "===========================================================" << endl;
	result = cascade.GetDetection(image, 12.0 / min_face_size, 0.7, true, 0.7, true, points);
	p1 = std::chrono::system_clock::now();
	cout << "detection time:" << (float)std::chrono::duration_cast<std::chrono::microseconds>(p1 - p0).count() / 1000 << "ms" << endl;

	for (int i = 0; i < result.size(); i++) {
		//cout << "face box:" << result[i].first << " confidence:" << result[i].second << endl;
		rectangle(image, result[i].first, Scalar(255, 0, 0), 2);
		if (points.size() >= i + 1) {
			for (int p = 0; p < 5; p++) {
				circle(image, points[i][p], 2, Scalar(0, 255, 255), -1);
			}
		}
	}
	while (image.cols > 1000) {
		resize(image, image, Size(0, 0), 0.75, 0.75);
	}
	imshow("final", image);
	waitKey(0);

	delete kCaffeBinding;
	system("pause");

	return 0;
}