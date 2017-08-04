#pragma once//有 pragma once ，所以不加#ifndef   
#include <opencv2/opencv.hpp>
#include <iostream>

class CameraAbnormalDetector {
public:
	bool Check_Camera_Obscured(cv::Mat mImgSrc, int nThreshold);

};