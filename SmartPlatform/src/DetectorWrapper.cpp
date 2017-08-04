#include "DetectorWrapper.h"
#include "opencv2\opencv.hpp"
#include <memory>

using namespace cv;
using namespace std;

DetectorWrapper::DetectorWrapper()
{
	cv::Mat img_background = cv::imread("./data/model/background.bmp", 1);
	cv::Mat img_roi = img_background(cv::Range(150, 800), cv::Range(200, 1100));
	const int front_background_diff_threhold = 60;
	detector_ = std::shared_ptr<Detector>(new Detector(img_roi, 190, 550, 800, 650, 60, true));
}


DetectorWrapper::~DetectorWrapper()
{
}

bool DetectorWrapper::detect(cv::Mat img) {
	if (detector_) {
		cv::Mat img_roi = img(cv::Range(150, 800), cv::Range(200, 1100));
		return detector_->ProcessSegmentForward(img_roi);
	}
	else {
		return false;
	}
}

cv::Mat DetectorWrapper::get_segment_image() {
	if (detector_) {
		return detector_->GetSegmentImage();
	}
	else {
		return Mat();
	}
}

cv::Point* DetectorWrapper::get_result_point() {
	if (detector_) {
		cv::Point *p = detector_->GetResultPoint();
		for (int i = 0; i < 4; i++) {
			p->x = p->x + 200;
			p->y = p->y + 150;
			++p;
		}
		return p;
	}
	return nullptr;
}