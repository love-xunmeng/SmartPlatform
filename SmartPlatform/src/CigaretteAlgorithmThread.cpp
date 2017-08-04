#include "CigaretteAlgorithmThread.h"
#include "Common.h"
#include "Config.h"
#include "Detector.h"
#include "ImageSaveThread.h"
#include "MessageWithSession.h"
#include "MLog.h"
#include "SmartPlatform.pb.h"
#include "opencv2/opencv.hpp"
#include "boost/function/function0.hpp"
#include <iostream>
#include <sstream>
#include <time.h>
#include "Windows.h"

using namespace cv;
using namespace SmartPlatform;
using namespace std;

enum CigaregatteMoveDirection {
	Left = 0,
	Right = 1,
};

CigaretteAlgorithmThread::CigaretteAlgorithmThread(std::string thread_name)
	: is_run_(false)
	, thread_name_(thread_name)
	, message_queue_(new MessageQueue())
{
	//cv::Mat img_background = cv::imread("./data/model/background.bmp", 1);
	//cv::Mat img_roi = img_background(cv::Range(150, 800), cv::Range(200, 1100));
	//const int front_background_diff_threhold = 60;
	//detector_ = std::shared_ptr<Detector>(new Detector(img_roi, 190, 550, 800, 650, 60, true));
	//recognizer_ = std::shared_ptr<Recognizer>(new Recognizer());
}


CigaretteAlgorithmThread::~CigaretteAlgorithmThread()
{
}

int CigaretteAlgorithmThread::start() {
	is_run_ = true;
	boost::function0<void> f = boost::bind(&CigaretteAlgorithmThread::process, this);
	thread_ = boost::thread(f);
	return 0;
}

void CigaretteAlgorithmThread::stop() {
	if (is_run_)
	{
		is_run_ = false;
		if (thread_.joinable())
		{
			thread_.join();
		}
	}
}

void CigaretteAlgorithmThread::process() {
	cv::Mat img_background = cv::imread("./data/model/background.bmp", 1);
	cv::Mat img_roi = img_background(cv::Range(150, 800), cv::Range(200, 1100));
	const int front_background_diff_threhold = 80;
	detector_ = std::shared_ptr<Detector>(new Detector(img_roi, 190, 550, 800, 650, 80, true));
	recognizer_ = std::shared_ptr<Recognizer>(new Recognizer());
	while (is_run_)
	{
		std::shared_ptr<MessageWithSession> message_width_session = message_queue_->pop();
		std::shared_ptr<SmartPlatform::Message> message = message_width_session->message_;
		boost::shared_ptr<Session> session = message_width_session->session_;
		SmartPlatform::CigaretteAlgorithmRequest request = message->request().cigarette_algorithm_request();

		ostringstream ss;
		ss << thread_name_ << " processing: " << request.cigarette_id() << endl;

		if ((0 == request.color_image_width()) || (0 == request.color_image_height()) || (0 == request.color_image_bit_count()) 
			|| (0 == request.color_image().length())
			|| (request.color_image_width() * request.color_image_height() * (request.color_image_bit_count() / 8) != request.color_image().length()))
		{
			ss.str("");;
			ss << request.cigarette_id() << ": Invalid color image." << endl;
			MLogger.error(ss.str());
			
			//是否应该把cigarette_id也返回？
			process_invalid_image_data(session);

			ss.str("");;
			ss << "Finish processing " << request.cigarette_id() << endl;
			MLogger.info(ss.str());

			
			continue;
		}

		int image_type = request.color_image_bit_count() / 8;
		cv::Mat img_color;
		switch (image_type)
		{
		case 1: //gray
			break;
		case 2:
			break;
		case 3: //rgb
			img_color = Mat(request.color_image_height(), request.color_image_width(), CV_8UC3);
			memcpy(img_color.data, request.color_image().c_str(), request.color_image_height() * request.color_image_width() * image_type);
			break;
		case 4: //argb
			break;
		default:
			break;
		}

		static long total_time = 0;
		static long count = 0;

		cv::Mat img_roi = img_color(cv::Range(150, 800), cv::Range(200, 1100));
		bool detect_result = false;
		try {
			detect_result = detector_->ProcessSegmentForward(img_roi);
		}
		catch (cv::Exception &e) {
			detect_result = false;
			MLogger.error("cv::Excption: " + string(e.what()));
		}
		catch (...) {
			detect_result = false;
			MLogger.error("Unkown error.");
		}

		if (Config::instance()->is_save_image()) {
			ImageSaveThread::instance()->save(img_color);
		}

		if (detect_result)
		{
			cv::Mat img_cigarette = detector_->GetSegmentImage();
			DWORD tic_start = GetTickCount();
			int predict_id = recognizer_->recognize(img_cigarette);
			DWORD tic_end = GetTickCount();
			cout << "Recognize Time elapsed: " << tic_end - tic_start << endl;
			int db_id = Config::instance()->get_db_id_by_train_id(predict_id);
			ss.str("");
			ss << "\npredict_id=" << predict_id << ", db_id=" << db_id << endl;
			MLogger.info(ss.str());
			process_detect_success(message, session, db_id); 
		}
		else {
			//ss.str("");;
			//ss << request.cigarette_id() << " detect fail.";
			//MLogger.warning(ss.str());

			//ss.str("");;
			//ss << "./data/tmp/DetectFail/" << request.cigarette_id() << ".bmp";
			//string path = ss.str();
			
			process_detect_fail(message, session);

			//ss.str("");;
			//ss << "Finish processing " << request.cigarette_id() << endl;
			//MLogger.info(ss.str());
		}

		//ss.str("");;
		//ss << "Finish processing " << request.cigarette_id() << endl;
		//MLogger.info(ss.str());
	}
}

void CigaretteAlgorithmThread::execute(const std::shared_ptr<MessageWithSession> &message) {
	message_queue_->push(message);
}

void CigaretteAlgorithmThread::process_invalid_image_data(boost::shared_ptr<Session> session) {
	SmartPlatform::Message message;
	message.set_version_code("V0.00001");
	message.set_type(SmartPlatform::CigaretteAlgorithm_Response);
	message.set_sequence(1);
	SmartPlatform::Response *response = message.mutable_response();
	response->set_state_code(SmartPlatform::BadRequest);
	response->set_state_desc("Be sure image width, height, color_image_bit_count and image data'size is correst.");
	std::string str_message;
	message.SerializeToString(&str_message);
	session->write(str_message);
}

void CigaretteAlgorithmThread::process_detect_success(std::shared_ptr<SmartPlatform::Message> message, boost::shared_ptr<Session> session, int db_id) {
	cv::Point *pointResult = detector_->GetResultPoint();
	//Fixme:
	cv::Point *p = pointResult;
	for (int i = 0; i < 4; i++) {
		p->x = p->x + 200;
		p->y = p->y + 150;
		++p;
	}
	//

	SmartPlatform::Point *left_down = new SmartPlatform::Point();
	left_down->set_x(pointResult[0].x);
	left_down->set_y(pointResult[0].y);
	SmartPlatform::Point *left_up = new SmartPlatform::Point();
	left_up->set_x(pointResult[1].x);
	left_up->set_y(pointResult[1].y);
	SmartPlatform::Point *right_up = new SmartPlatform::Point();
	right_up->set_x(pointResult[2].x);
	right_up->set_y(pointResult[2].y);
	SmartPlatform::Point *right_down = new SmartPlatform::Point();
	right_down->set_x(pointResult[3].x);
	right_down->set_y(pointResult[3].y);

	SmartPlatform::CigaretteContour *contour = new SmartPlatform::CigaretteContour();
	contour->set_allocated_left_down(left_down);
	contour->set_allocated_left_up(left_up);
	contour->set_allocated_right_up(right_up);
	contour->set_allocated_right_down(right_down);

	SmartPlatform::CigaretteAlgorithmResponse *cigarette_algorithm_response = new SmartPlatform::CigaretteAlgorithmResponse();
	cigarette_algorithm_response->set_result_code(2000);
	cigarette_algorithm_response->set_cigarette_id(message->request().cigarette_algorithm_request().cigarette_id());
	cigarette_algorithm_response->set_type_id(db_id);
	cigarette_algorithm_response->set_allocated_cigarette_contour(contour);

	SmartPlatform::Response *response = new SmartPlatform::Response();
	response->set_state_code(SmartPlatform::Success);
	response->set_state_desc("Success");
	response->set_allocated_cigarette_algorithm_response(cigarette_algorithm_response);

	Message response_message;
	response_message.set_version_code("v0.000001");
	response_message.set_type(CigaretteAlgorithm_Response);
	response_message.set_sequence(1);
	response_message.set_allocated_response(response);

	std::string str_response;
	response_message.SerializeToString(&str_response);
	int response_message_length = str_response.length();
	unsigned char bytes[4];
	change_int_to_bytes(response_message_length, bytes);
	session->write(str_response);
}

void CigaretteAlgorithmThread::process_detect_fail(std::shared_ptr<SmartPlatform::Message> message, boost::shared_ptr<Session> session) {
	SmartPlatform::CigaretteAlgorithmResponse *cigarette_algorithm_response = new SmartPlatform::CigaretteAlgorithmResponse();
	cigarette_algorithm_response->set_result_code(4000);//4000: 分割失败
	cigarette_algorithm_response->set_cigarette_id(message->request().cigarette_algorithm_request().cigarette_id());

	SmartPlatform::Response *response = new SmartPlatform::Response();
	response->set_state_code(Success);
	response->set_state_desc("Success");
	response->set_allocated_cigarette_algorithm_response(cigarette_algorithm_response);

	SmartPlatform::Message message_response;
	message_response.set_version_code("v0.00001");
	message_response.set_type(CigaretteAlgorithm_Response);
	message_response.set_sequence(1);
	message_response.set_allocated_response(response);

	std::string str_response;
	message_response.SerializeToString(&str_response);
	int response_message_length = str_response.length();
	unsigned char bytes[4];
	change_int_to_bytes(response_message_length, bytes);
	//session->write(bytes, 4);
	session->write(str_response);
}

void CigaretteAlgorithmThread::change_int_to_bytes(int target, unsigned char *buffer) {
	memset(buffer, 0, sizeof(unsigned char) * 4);
	buffer[0] = (unsigned char)(0xff & target);
	buffer[1] = (unsigned char)((0xff00 & target) >> 8);
	buffer[2] = (unsigned char)((0xff0000 & target) >> 16);
	buffer[3] = (unsigned char)((0xff000000 & target) >> 24);
}
