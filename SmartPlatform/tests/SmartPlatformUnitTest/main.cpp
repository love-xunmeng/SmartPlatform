#include "Detector.h"
#include "gtest/gtest.h"
#include "opencv2\opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int add(int a, int b) {
	return a + b;
}

TEST(fun, add) {
	EXPECT_EQ(1, add(2, -1));
}

int main(int argc, char** argv) {
	cv::Mat img_background = imread("background.bmp");
	cv::Mat img_roi_background = img_background(cv::Range(70, 870), cv::Range(300, 1200));
	Detector detecter(img_roi_background);
	cv::Mat img_test = imread("2.bmp");
	while (true) {
		cv::Mat img_roi  = img_test(cv::Range(70, 870), cv::Range(300, 1200));
		bool detect_result = detecter.ProcessSegmentForward(img_roi);
	}
}