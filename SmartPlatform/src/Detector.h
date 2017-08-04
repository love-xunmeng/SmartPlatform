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
	// Detector:				���캯��
	// ˵��:					������������������̷ָ����
	// matBackMode:	���뱳��ͼ��
	// nCigarWidth:		�����̿��
	// nCigarHeight:		�����̸߶�
	//nAreaWidth:			���������
	//nAreaHeight:		�������߶�
	//nThSegment:		ǰ�������ָ���ֵ���Ƽ����ã�60
	//nMoveDirect:		���з���true�����򣬴������ң�false�����򣬴�������
	Detector(cv::Mat matBackMode, int nCigarWidth, int nCigarHeight, int nAreaWidth, int nAreaHeight , int nThSegment, bool nMoveDirect);
	~Detector();

public:
	/****************************************************************/
	// ProcessSegmentForward: ����ָ��
	// ˵��:					��������£�������Դͼ����зָ�
	// matSrc:				����ָ�ͼ��
	//����ֵ��				true���ɹ��ָ�ɻ�ȡ�ָ�����false���ָ�ʧ�ܻ��߲���Ҫ�ָ�(�ظ��ָ�)
	bool ProcessSegmentForward(cv::Mat &matSrc);
	/****************************************************************/
	// ProcessSegmentBack: ����ָ��
	// ˵��:					��������£�������Դͼ����зָ�
	// matSrc:				����ָ�ͼ��
	//����ֵ��				true���ɹ��ָ�ɻ�ȡ�ָ�����false���ָ�ʧ�ܻ��߲���Ҫ�ָ�(�ظ��ָ�)
	bool ProcessSegmentBack(cv::Mat &matSrc);
	/*******************************************/
	// GetBwImage: ��ȡ�ָ�Դͼ��Ķ�ֵͼ��
	//����ֵ��			��ֵͼ��
	cv::Mat GetBinaryImage() { return m_matBinary; };
	/***********************************************/
	// GetResultImage: ��ȡ�ָ��ĵ�����ͼ��
	//����ֵ��			������ͼ��
	cv::Mat GetSegmentImage() { return m_matSegment; };
	/***********************************************/
	// GetSegmentImage: ��ȡ�ָ�Դͼ��ľ��α�עͼ��
	//����ֵ��			���α�עͼ��
	cv::Mat GetResultImage() { return m_matResult; };
	/***********************************************/
	// GetResultPoint: ��ȡ�ָ��ĵ����̵��ĸ���������
	//����ֵ��			��������
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