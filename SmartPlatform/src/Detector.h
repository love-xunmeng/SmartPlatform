#ifndef Segment_H_
#define Segment_H_

#include "opencv/cv.h"
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <math.h>

#define  SPEED_VALUE 130
#define  ERROR_VALUE 100

struct SBasicParameter
{
	int s_nCigarWidth;
	int s_nCigarHeight;
	int s_nAreaWidth;
	int s_nAreaHeight;
	int s_nSpeedValue;
	int s_nErrorValue;
	SBasicParameter()
	{
		s_nCigarWidth = 220;
		s_nCigarHeight = 720;
		s_nAreaWidth = 800;
		s_nAreaHeight = 800;
		s_nSpeedValue = 130;
		s_nErrorValue = 100;
	}
};

class Detector
{
public:
	/****************************************************************/
	// Detector:				构造函数
	// 说明:					根据输入参数构造条烟分割对象
	// matBackMode:	输入背景图像
	// nCigarWidth:		单条烟宽度
	// nCigarHeight:		单条烟高度
	//nAreaWidth:			检测区域宽度
	//nAreaHeight:		检测区域高度
	//nThSegment:		前景背景分割阈值，推荐设置：60
	//nMoveDirect:		运行方向，true：正向，从左至右；false：反向，从右至左；
	Detector(cv::Mat matBackMode, int nCigarWidth, int nCigarHeight, int nAreaWidth, int nAreaHeight , int nThSegment, bool nMoveDirect);
	~Detector();

public:
	/****************************************************************/
	// ProcessSegmentForward: 正向分割处理
	// 说明:					正向情况下，对输入源图像进行分割
	// matSrc:				输入分割图像
	//返回值：				true：成功分割，可获取分割结果；false：分割失败或者不需要分割(重复分割)
	bool ProcessSegmentForward(cv::Mat &matSrc);
	/****************************************************************/
	// ProcessSegmentBack: 反向分割处理
	// 说明:					反向情况下，对输入源图像进行分割
	// matSrc:				输入分割图像
	//返回值：				true：成功分割，可获取分割结果；false：分割失败或者不需要分割(重复分割)
	bool ProcessSegmentBack(cv::Mat &matSrc);
	/*******************************************/
	// GetBwImage: 获取分割源图像的二值图像
	//返回值：			二值图像
	cv::Mat GetBinaryImage() { return m_matBinary; };
	/***********************************************/
	// GetResultImage: 获取分割后的单条烟图像
	//返回值：			单条烟图像
	cv::Mat GetSegmentImage() { return m_matSegment; };
	/***********************************************/
	// GetSegmentImage: 获取分割源图像的矩形标注图像
	//返回值：			矩形标注图像
	cv::Mat GetResultImage() { return m_matResult; };
	/***********************************************/
	// GetResultPoint: 获取分割后的单条烟的四个顶点坐标
	//返回值：			顶点坐标
	cv::Point* GetResultPoint() { return m_pointNowVertex; }

private:

	void RgbSegment(cv::Mat& matSrc);
	void RgbHsvSegment(cv::Mat& matSrc);

	void NormImage(cv::Mat& matSrc);
	void DenoiseImg();
	void FindValidData();
	void FilterSmallRect();

	bool SegmentAreaForward();
	bool SegmentAreaBack();
	void LargeAreaSegForward();
	void LargeAreaSegBack();

	int FindClose(cv::Point pointValue);
	void FindClose(cv::Point pointValue, int& sizeFir, int& sizeSec);

	void RectToPoint(cv::Point* pointValue, cv::Rect rectValue);
	void PointToRect();

	void UpToOldPoint();
	void DownToOldPoint();

	void FindMaxXArea();
	void FindMinXArea();

	void RectToPointRToL(cv::Point* pointOld);
	void RectToPointLToR(cv::Point* pointOld);
	void PointToPointRToL(cv::Point* pointOld);
	void PointToPointLToR(cv::Point* pointOld);
	cv::Point Update_1(cv::Point pointValue);
	void Update_2(cv::Point& pointValue);

	void AffineTransform(cv::Mat& matImage, cv::Rect rectValue, cv::Point* pPointInput);
	void FindRectAreaPoint(cv::Point2f pPoint[4], cv::Point2f pPointDst[4]);
	bool ValiadResultPoint();
	void FindMaxOfSmallSizes();
	static bool SortRect(const cv::Rect &Rect1, const cv::Rect &Rect2);
	void DeleteRect();
	bool GatherAreaFirst();
	bool GatherAreaSecond();
	bool ComputeVertex();

private:
	int m_nCigarWidth, m_nCigarHeight, m_nAreaWidth, m_nAreaHeight;

	int m_nThSegment;
	double m_dThMinValue, m_dThMaxValue;
	double m_dThLargeArea;

	int m_nRowNumber, m_nColNumber;
	int m_nIndex, m_nFirst, m_nSecond;
	int m_nSmallAreaNum;
	double m_dMaxSmallSizes;


	cv::Point m_pointVertex[4];
	cv::Point m_pointOldVertex[4];
	cv::Point m_pointNowVertex[4];

	cv::Mat m_matBackMode;
	cv::Mat m_matSegment;
	cv::Mat m_matBinary;
	cv::Mat m_matResult;
	cv::Rect m_rectResult;

	std::vector<cv::Mat> m_vecMatBGR;
	std::vector<cv::Point> m_vecPointAcon;
	std::vector<cv::Rect> m_vecRectLarge, m_vecRectSmall;
	std::vector<double>  m_vecSizeLarge, m_vecSizeSmall;
	std::vector<std::vector<cv::Point>> m_vecContoursSmall, m_vecContoursLarge;
};

#endif