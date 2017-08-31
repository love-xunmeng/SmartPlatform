#include "opencv2/opencv.hpp"
#include "CameraObscured.h"
using namespace cv;

//************************************************************************//
//Function: 
//         Check the camera is been obscured or not
//Input Param:
//          Mat mImgSrc &      : input image
//			int nCheckThread   : the thread of check camera been obscured
//
//Output Param:
//Return Param:
//			bool bCameraObscured : if obscured return true.
//**************************************************************************//
bool  CameraAbnormalDetector::Check_Camera_Obscured(Mat mImgSrc, int nThreshold)
{
	Mat mInputImage, mGrayImage;
	Mat mCannyResult;
	//Record gradient information
	Mat mGrad_x, mGrad_y, mAbs_Grad_x, mAbs_Grad_y;
	Mat mGrad;
	if (mImgSrc.empty())
	{
		//exit(1);//用这样的方式会导致整个程序退出，陈健斌发现的。改为return false
		return false;
	}
	int nWidth = mImgSrc.cols;
	int nHight = mImgSrc.rows;
	bool bCameraObscured = false;

	//Step 1 : medianBlur remove noise
	medianBlur(mImgSrc, mInputImage, 3);

	//Step 2 : RGB to Gray
	cvtColor(mInputImage, mGrayImage, CV_RGB2GRAY);

	//Step 3 : Use canny() get the edge information of image
	Canny(mGrayImage, mCannyResult, 140, 220);

	//Step 4 : Get the gradient value of Gray image
	Sobel(mCannyResult, mGrad_x, mGrayImage.depth(), 1, 0, 3, BORDER_DEFAULT);
	convertScaleAbs(mGrad_x, mAbs_Grad_x);
	Sobel(mCannyResult, mGrad_y, mGrayImage.depth(), 0, 1, 3, BORDER_DEFAULT);
	convertScaleAbs(mGrad_y, mAbs_Grad_y);
	addWeighted(mAbs_Grad_x, 0.5, mAbs_Grad_y, 0.5, 0, mGrad);

	//Step 5 : Get the sum and average of gradient image and judge camera
	Scalar sSumOfGrad = sum(mGrad);
	double dSumOfGrad = sSumOfGrad.val[0];
	double dAvarageOfGrad = dSumOfGrad / (nWidth * nHight);
	std::cout << "dAvarageOfGrad=" << dAvarageOfGrad << std::endl;
	if (static_cast<int>(dAvarageOfGrad) < nThreshold)
	{
		bCameraObscured = true;
	}
	else
	{
		bCameraObscured = false;
	}

	return bCameraObscured;
}