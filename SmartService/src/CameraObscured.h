#pragma once//�� pragma once �����Բ���#ifndef   
#include <opencv2/opencv.hpp>
#include <iostream>

class CameraAbnormalDetector {
public:
	bool Check_Camera_Obscured(cv::Mat mImgSrc, int nThreshold);

};