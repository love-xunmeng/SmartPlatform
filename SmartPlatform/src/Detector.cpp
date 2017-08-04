#include "Detector.h"

Detector::Detector(cv::Mat matBackMode, int nCigarWidth, int nCigarHeight, int nAreaWidth, int nAreaHeight, int nThSegment, bool nMoveDirect )
{
	m_matBackMode = matBackMode.clone();
	m_nRowNumber = m_matBackMode.rows;
	m_nColNumber = m_matBackMode.cols;
	m_nThSegment = nThSegment;
	m_nCigarWidth = nCigarWidth;
	m_nCigarHeight = nCigarHeight;
	m_nAreaWidth = nAreaWidth;
	m_nAreaHeight = nAreaHeight;
	m_dThMinValue = 0.05*m_nCigarWidth*m_nCigarHeight;
	m_dThMaxValue = 0.7*m_nCigarWidth*m_nCigarHeight;
	m_dThLargeArea = 1.5*m_nCigarWidth*m_nCigarHeight;
	if (true == nMoveDirect)
	{
		m_pointOldVertex[0] = cv::Point(m_nAreaWidth, m_nAreaHeight);
		m_pointOldVertex[1] = cv::Point(m_nAreaWidth, 1);
		m_pointOldVertex[2] = cv::Point(m_nAreaWidth + m_nCigarWidth, 1);
		m_pointOldVertex[3] = cv::Point(m_nAreaWidth + m_nCigarWidth, m_nAreaHeight);
	}
	else
	{
		m_pointOldVertex[0] = cv::Point(1 - m_nCigarWidth, m_nAreaHeight);
		m_pointOldVertex[1] = cv::Point(1 - m_nCigarWidth, 1);
		m_pointOldVertex[2] = cv::Point(1, 1);
		m_pointOldVertex[3] = cv::Point(1, m_nAreaHeight);
	}
	
	//NormImage(m_matBackMode);
}
Detector::~Detector()
{
}

void Detector::RgbSegment(cv::Mat& matSrc)
{
	int nTemp = 0;
	uchar *pData, *pDataR, *pDataG, *pDdataB;
	cv::Mat matSrcCopy = matSrc.clone();
	m_matBinary = cv::Mat::zeros(m_nRowNumber, m_nColNumber, CV_8UC1);
	cv::Mat matImgDiff = abs(matSrcCopy - m_matBackMode) + abs(m_matBackMode - matSrcCopy);
	cv::split(matImgDiff, m_vecMatBGR);
	pData = m_matBinary.data;
	pDdataB = m_vecMatBGR[0].data;
	pDataG = m_vecMatBGR[1].data;
	pDataR = m_vecMatBGR[2].data;
	int nImgSize = (int)matSrcCopy.total();
	for (int nIndex = 0; nIndex < nImgSize; ++nIndex)
	{
		nTemp = pDdataB[nIndex] + pDataG[nIndex] + pDataR[nIndex];
		if (nTemp > 60)
		{
			pData[nIndex] = 255;
		}
	}
	cv::morphologyEx(m_matBinary, m_matBinary, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 7)));
	cv::morphologyEx(m_matBinary, m_matBinary, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 7)));
}

void Detector::NormImage(cv::Mat& matSrc)
{
	int nTemp = 0;
	uchar uMaxTemp = 50;
	uchar  *pDataR, *pDataG, *pDdataB;
	split(matSrc, m_vecMatBGR);
	for (int i = 0; i<m_nRowNumber; i++)
	{
		pDdataB = m_vecMatBGR[0].ptr<uchar>(i);
		pDataG = m_vecMatBGR[1].ptr<uchar>(i);
		pDataR = m_vecMatBGR[2].ptr<uchar>(i);
		for (int j = 0; j<m_nColNumber; j++)
		{
			nTemp = std::min(std::min(std::min(pDdataB[j], pDataG[j]), pDataR[j]), uMaxTemp);  
			pDdataB[j] = pDdataB[j] - nTemp;
			pDataG[j] = pDataG[j] - nTemp;
			pDataR[j] = pDataR[j] - nTemp;
		}
	}
	merge(m_vecMatBGR, matSrc);
}

void Detector::RgbHsvSegment(cv::Mat& matSrc)
{
	int nTemp;
	cv::Mat matHsv;
	std::vector<cv::Mat> vecMatHsv;
	uchar *pDataH, *pDataS, *pDataV;
	uchar *pData, *pDataR, *pDataG, *pDdataB;
	cvtColor(matSrc, matHsv, CV_BGR2HSV);
	split(matHsv, vecMatHsv);
	for (int i = 0; i<m_nRowNumber; i++)
	{
		pDdataB = m_vecMatBGR[0].ptr<uchar>(i);
		pDataG = m_vecMatBGR[1].ptr<uchar>(i);
		pDataR = m_vecMatBGR[2].ptr<uchar>(i);
		pDataH = vecMatHsv[0].ptr<uchar>(i);
		pDataS = vecMatHsv[1].ptr<uchar>(i);
		pDataV = vecMatHsv[2].ptr<uchar>(i);
		pData = m_matBinary.ptr<uchar>(i);
		for (int j = 0; j<m_nColNumber; j++)
		{
			nTemp = pDdataB[j] + pDataG[j] + pDataR[j];
			if (nTemp>m_nThSegment || (((pDataH[j] >= 0 && pDataH[j] <= 8) || (pDataH[j] >= 160 && pDataH[j] <= 180)) && pDataS[j]>70 && (pDataV[j]>80 && pDataV[j]<220)))
				pData[j] = 255;
			else
				pData[j] = 0;
		}
	}
}

void Detector::DenoiseImg()
{
	CvSeq *pContours = NULL;
	CvMemStorage *pStorage = cvCreateMemStorage(0);
	cv::Mat matBWTemp = m_matBinary.clone();
	IplImage iplImg = matBWTemp;
	cvFindContours(&iplImg, pStorage, &pContours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	m_matBinary = cv::Mat::zeros(m_nRowNumber, m_nColNumber, CV_8UC1);
	IplImage iplImg2 = m_matBinary;
	for (; pContours != 0; pContours = pContours->h_next)
	{
		if (cvContourArea(pContours)>m_dThMinValue)
		{
			cvDrawContours(&iplImg2, pContours, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, CV_FILLED);
		}
	}
	//erode(m_matBinary, m_matBinary, getStructuringElement(cv::MORPH_RECT, cv::Size(3, 7)));
	cv::morphologyEx(m_matBinary, m_matBinary, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 7)));
	cv::morphologyEx(m_matBinary, m_matBinary, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 7)));
}

void Detector::FindValidData()
{
	m_vecContoursSmall.clear();
	m_vecContoursLarge.clear();
	m_vecSizeSmall.clear();
	m_vecSizeLarge.clear();
	m_vecRectSmall.clear();
	m_vecRectLarge.clear();
	CvSeq *pContours = NULL;
	CvMemStorage *pStorage = cvCreateMemStorage(0);
	std::vector<cv::Point> vecPoint;
	cv::Mat matBWTemp = m_matBinary.clone();
	IplImage iplImg = matBWTemp;
	double dArea;
	cvFindContours(&iplImg, pStorage, &pContours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (; pContours != 0; pContours = pContours->h_next)
	{
		dArea = cvContourArea(pContours);
		if (dArea>m_dThMinValue)
		{
			vecPoint.clear();
			for (int iCount = 0; iCount<pContours->total; iCount++)
			{
				vecPoint.push_back(*(CvPoint*)cvGetSeqElem(pContours, iCount));
			}
			if (dArea>m_dThMaxValue)
			{
				m_vecContoursLarge.push_back(vecPoint);
				m_vecSizeLarge.push_back(dArea);
				m_vecRectLarge.push_back(boundingRect(vecPoint));
			}
			else
			{
				m_vecContoursSmall.push_back(vecPoint);
				m_vecSizeSmall.push_back(dArea);
				m_vecRectSmall.push_back(boundingRect(vecPoint));
			}
		}
	}
	if (pStorage) {
		cvReleaseMemStorage(&pStorage);
		pStorage = nullptr;
	}
	FilterSmallRect();
}

void Detector::FilterSmallRect()
{
	m_nSmallAreaNum = m_vecRectSmall.size();
	if (m_vecRectSmall.size()>0)
	for (size_t i = 0; i<m_vecRectSmall.size(); i++)
	{
		if (m_vecRectSmall[i].x + m_vecRectSmall[i].width<m_nCigarWidth || m_vecRectSmall[i].x>(m_nAreaWidth - m_nCigarWidth))//删除两边的目标
			m_nSmallAreaNum--;
		else if (m_vecRectSmall[i].y + m_vecRectSmall[i].height<50) //可以删除顶部目标
			m_nSmallAreaNum--;
	}
	/*
	if (m_vecRectSmall.size()>0&&m_vecRectLarge.size()>0)
	for (size_t i=0;i<m_vecRectSmall.size();i++)
	for (size_t j=0;j<m_vecRectLarge.size();j++)
	{
	if ((m_vecRectSmall[i]&m_vecRectLarge[j])==m_vecRectLarge[j])
	m_nSmallAreaNum--;
	}*/
}
bool Detector::ProcessSegmentForward(cv::Mat &matSrc)
{
	if (matSrc.empty())
	{
		return false;
	}
	m_matResult = matSrc.clone();
	RgbSegment(matSrc);
	FindValidData();           //提取连通分量有效数据

	if (m_vecRectLarge.size() > 0)
	{
		if (true == SegmentAreaForward())
		{
			PointToRect();
			AffineTransform(matSrc(m_rectResult).clone(), m_rectResult, m_pointOldVertex);
			rectangle(m_matResult, m_rectResult, cv::Scalar(0, 255, 255), 3);
			for (int i = 0; i < 4; i++)
			{
				circle(m_matResult, m_pointOldVertex[i], 10, cv::Scalar(0, 0, 255), 5);
				m_pointNowVertex[i] = m_pointOldVertex[i];
			}
			UpToOldPoint();
			return true;
		}
	}
	UpToOldPoint();
	return false;
}

bool Detector::ProcessSegmentBack(cv::Mat &matSrc)
{
	if (matSrc.empty())
	{
		return false;
	}

	m_matResult = matSrc.clone();
	RgbSegment(matSrc);
	FindValidData();           //提取连通分量有效数据
					
	if (m_vecRectLarge.size() > 0)
	{
		 if (true == SegmentAreaBack())
		 {
			 PointToRect();
			 AffineTransform(matSrc(m_rectResult).clone(), m_rectResult, m_pointOldVertex);
			 rectangle(m_matResult, m_rectResult, cv::Scalar(0, 255, 255), 3);
			 for (int i = 0; i < 4; i++)
			 {
				 circle(m_matResult, m_pointOldVertex[i], 10, cv::Scalar(0, 0, 255), 5);
				 m_pointNowVertex[i] = m_pointOldVertex[i];
			 }
			 DownToOldPoint();
			return true;
		 }
	 }
	DownToOldPoint();
	return false;
}


bool Detector::SegmentAreaForward()
{
	FindMaxXArea();//找到X坐标最大的区域，即最右端区域
	RectToPoint(m_pointVertex, m_vecRectLarge[m_nIndex]);
	approxPolyDP(m_vecContoursLarge[m_nIndex], m_vecPointAcon, 10, true);
	cv::Point2f pRotateRectTemp[4], pRotateRect[4];
	// 保证检测到的区域(最右端区域)距离上次检测结果之间存在一条烟的距离，避免重复检测，否则跳到else,舍去这一区域从新搜索
	//避免重复检测(区域之间的重复检测,主要是小区域和小区域，大区域和小区域，以及大区域与大区域)
	if ((m_vecPointAcon[FindClose(m_pointVertex[0])].x + m_nCigarWidth)<(m_pointOldVertex[0].x + ERROR_VALUE) && (m_vecPointAcon[FindClose(m_pointVertex[1])].x + m_nCigarWidth)<(m_pointOldVertex[1].x + ERROR_VALUE))
	{
		if (((m_pointVertex[2].x>2 * m_nCigarWidth || m_pointVertex[3].x>2 * m_nCigarWidth) && m_pointVertex[0].x>5 && m_pointVertex[1].x>5) || m_pointVertex[2].x>3 * m_nCigarWidth || m_pointVertex[3].x>3 * m_nCigarWidth)//????????????
		{
			if (m_vecSizeLarge[m_nIndex] > m_dThLargeArea)
			{
				LargeAreaSegForward();
			}
			else//输出分割结果
			{
// 				m_pointOldVertex[0] = m_vecPointAcon[FindClose(m_pointVertex[0])];
// 				m_pointOldVertex[1] = m_vecPointAcon[FindClose(m_pointVertex[1])];
// 				m_pointOldVertex[2] = m_vecPointAcon[FindClose(m_pointVertex[2])];
// 				m_pointOldVertex[3] = m_vecPointAcon[FindClose(m_pointVertex[3])];
				cv::RotatedRect rotateRect = cv::minAreaRect(m_vecPointAcon);
				rotateRect.points(pRotateRectTemp);
				FindRectAreaPoint(pRotateRectTemp, pRotateRect);
				m_pointOldVertex[0] = pRotateRect[0];
				m_pointOldVertex[1] = pRotateRect[1];
				m_pointOldVertex[2] = pRotateRect[2];
				m_pointOldVertex[3] = pRotateRect[3];
			}
			if (ValiadResultPoint())
			{
				return true;
			}
		}
	}
	else if (m_vecRectLarge.size()>1)
	{
		m_vecRectLarge.erase(m_vecRectLarge.begin() + m_nIndex);
		m_vecSizeLarge.erase(m_vecSizeLarge.begin() + m_nIndex);
		m_vecContoursLarge.erase(m_vecContoursLarge.begin() + m_nIndex);
		FindMaxXArea();//找到X坐标最大的区域，即最右端区域
		RectToPoint(m_pointVertex, m_vecRectLarge[m_nIndex]);
		approxPolyDP(m_vecContoursLarge[m_nIndex], m_vecPointAcon, 10, true);
		if (((m_pointVertex[2].x>2 * m_nCigarWidth || m_pointVertex[3].x>2 * m_nCigarWidth) && m_pointVertex[0].x>5 && m_pointVertex[1].x>5) || m_pointVertex[2].x>3 * m_nCigarWidth || m_pointVertex[3].x>3 * m_nCigarWidth)
		{
			// 保证检测到的区域(最右端区域)距离上次检测结果之间存在一条烟的距离，避免重复检测
			//避免重复检测(区域之间的重复检测,主要是小区域和小区域，大区域和小区域，以及大区域与大区域), 新加的
			if ((m_vecPointAcon[FindClose(m_pointVertex[0])].x + m_nCigarWidth) < (m_pointOldVertex[0].x + ERROR_VALUE) && (m_vecPointAcon[FindClose(m_pointVertex[1])].x + m_nCigarWidth)<(m_pointOldVertex[1].x + ERROR_VALUE))
			{
				if (m_vecSizeLarge[m_nIndex] > m_dThLargeArea)//判断是不是大区域
				{
					LargeAreaSegForward();
				}
				else//输出分割结果
				{
					cv::RotatedRect rotateRect = cv::minAreaRect(m_vecPointAcon);
					rotateRect.points(pRotateRectTemp);
					FindRectAreaPoint(pRotateRectTemp, pRotateRect);
					m_pointOldVertex[0] = pRotateRect[0];
					m_pointOldVertex[1] = pRotateRect[1];
					m_pointOldVertex[2] = pRotateRect[2];
					m_pointOldVertex[3] = pRotateRect[3];
				}
				if (ValiadResultPoint())
				{
					return true;
				}
			}		
		}
	}
	return false;
}

bool Detector::SegmentAreaBack()
{
	FindMinXArea();//找到X坐标最小的区域，即最左端区域
	RectToPoint(m_pointVertex, m_vecRectLarge[m_nIndex]);
	approxPolyDP(m_vecContoursLarge[m_nIndex], m_vecPointAcon, 10, true);
	cv::Point2f pRotateRectTemp[4], pRotateRect[4];
	if ((m_vecPointAcon[FindClose(m_pointVertex[3])].x - m_nCigarWidth)>(m_pointOldVertex[3].x - ERROR_VALUE) && (m_vecPointAcon[FindClose(m_pointVertex[2])].x - m_nCigarWidth>(m_pointOldVertex[2].x - ERROR_VALUE)))
	{
		if (((m_pointVertex[0].x<2 * m_nCigarWidth || m_pointVertex[1].x<2 * m_nCigarWidth) && m_pointVertex[2].x<(m_nAreaWidth - 5) && m_pointVertex[3].x<(m_nAreaWidth - 5)) || (m_pointVertex[0].x<m_nCigarWidth || m_pointVertex[1].x<m_nCigarWidth))
		{
			if (m_vecSizeLarge[m_nIndex]>m_dThLargeArea){
				LargeAreaSegBack();
			}
			else//输出分割结果
			{
				cv::RotatedRect rotateRect = cv::minAreaRect(m_vecPointAcon);
				rotateRect.points(pRotateRectTemp);
				FindRectAreaPoint(pRotateRectTemp, pRotateRect);
				m_pointOldVertex[0] = pRotateRect[0];
				m_pointOldVertex[1] = pRotateRect[1];
				m_pointOldVertex[2] = pRotateRect[2];
				m_pointOldVertex[3] = pRotateRect[3];
			}
			if (ValiadResultPoint())
			{
				return true;
			}
		}
	}
	else if (m_vecRectLarge.size()>1)
	{
		m_vecRectLarge.erase(m_vecRectLarge.begin() + m_nIndex);
		m_vecSizeLarge.erase(m_vecSizeLarge.begin() + m_nIndex);
		m_vecContoursLarge.erase(m_vecContoursLarge.begin() + m_nIndex);
		FindMinXArea();
		RectToPoint(m_pointVertex, m_vecRectLarge[m_nIndex]);
		approxPolyDP(m_vecContoursLarge[m_nIndex], m_vecPointAcon, 10, true);
		if ((m_vecPointAcon[FindClose(m_pointVertex[3])].x - m_nCigarWidth)>(m_pointOldVertex[3].x - ERROR_VALUE) && (m_vecPointAcon[FindClose(m_pointVertex[2])].x - m_nCigarWidth>(m_pointOldVertex[2].x - ERROR_VALUE)))
		{
			if (((m_pointVertex[0].x<2 * m_nCigarWidth || m_pointVertex[1].x<2 * m_nCigarWidth) && m_pointVertex[2].x<(m_nAreaWidth - 5) && m_pointVertex[3].x<(m_nAreaWidth - 5)) || (m_pointVertex[0].x<m_nCigarWidth || m_pointVertex[1].x<m_nCigarWidth))
			{
				if (m_vecSizeLarge[m_nIndex]>m_dThLargeArea){
					LargeAreaSegBack();
				}
				else//输出分割结果
				{
					cv::RotatedRect rotateRect = cv::minAreaRect(m_vecPointAcon);
					rotateRect.points(pRotateRectTemp);
					FindRectAreaPoint(pRotateRectTemp, pRotateRect);
					m_pointOldVertex[0] = pRotateRect[0];
					m_pointOldVertex[1] = pRotateRect[1];
					m_pointOldVertex[2] = pRotateRect[2];
					m_pointOldVertex[3] = pRotateRect[3];
				}
				if (ValiadResultPoint())
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool Detector::ValiadResultPoint()
{
	if (abs(m_pointOldVertex[0].x - m_pointOldVertex[3].x) > m_nCigarWidth*0.5 && abs(m_pointOldVertex[1].x - m_pointOldVertex[2].x) > m_nCigarWidth*0.5
		&& abs(m_pointOldVertex[0].y - m_pointOldVertex[1].y) > m_nCigarHeight*0.5 && abs(m_pointOldVertex[3].y - m_pointOldVertex[2].y) > m_nCigarHeight*0.5)
	{
// 		if (m_pointOldVertex[0].x>0&& m_pointOldVertex[1].x>0 && m_pointOldVertex[2].x<m_nAreaWidth&&m_pointOldVertex[3].x<m_nAreaWidth
// 			&& m_pointOldVertex[1].y>0 &&m_pointOldVertex[2].y>0&& m_pointOldVertex[0].y<m_nAreaHeight&&m_pointOldVertex[3].y<m_nAreaHeight)
		{
			return true;
		}
	}
	return false;
}
void Detector::FindRectAreaPoint(cv::Point2f pPoint[4], cv::Point2f pPointDst[4])
{
	cv::Point pTemp = 0;
	if (pPoint[0].y < pPoint[1].y) { pTemp = pPoint[0]; pPoint[0] = pPoint[1]; pPoint[1] = pTemp; }
	if (pPoint[0].y < pPoint[2].y) { pTemp = pPoint[0]; pPoint[0] = pPoint[2]; pPoint[2] = pTemp; }
	if (pPoint[0].y < pPoint[3].y) { pTemp = pPoint[0]; pPoint[0] = pPoint[3]; pPoint[3] = pTemp; }
	if (pPoint[1].y < pPoint[2].y) { pTemp = pPoint[1]; pPoint[1] = pPoint[2]; pPoint[2] = pTemp; }
	if (pPoint[1].y < pPoint[3].y) { pTemp = pPoint[3]; pPoint[3] = pPoint[1]; pPoint[1] = pTemp; }
	if (pPoint[2].y < pPoint[3].y) { pTemp = pPoint[2]; pPoint[2] = pPoint[3]; pPoint[3] = pTemp; }
	if (pPoint[0].x < pPoint[1].x) 
	{
		pPointDst[0] = pPoint[0]; pPointDst[3] = pPoint[1];
	}
	else 
	{
		pPointDst[0] = pPoint[1]; pPointDst[3] = pPoint[0];
	}
	if (pPoint[2].x < pPoint[3].x)
	{ 
		pPointDst[1] = pPoint[2]; pPointDst[2] = pPoint[3]; 
	}
	else
	{
		pPointDst[1] = pPoint[3]; pPointDst[2] = pPoint[2];
	}
}
void Detector::LargeAreaSegForward()
{
	cv::Point pointOld[4];
	if (m_pointVertex[2].x < (m_nAreaWidth - 5) && m_pointVertex[3].x < (m_nAreaWidth - 5))//区域右端未超出区域
	{
		RectToPointRToL(pointOld);  //从右开始推的第一个
		//新推出的第一条烟的最右端要小于上次推出的烟的最左端加一个差值，可以避免重复(大区域内重复)
		if (pointOld[2].x < (m_pointOldVertex[1].x + ERROR_VALUE) || pointOld[3].x < (m_pointOldVertex[0].x + ERROR_VALUE))
		{
			m_pointOldVertex[0] = pointOld[0];
			m_pointOldVertex[1] = pointOld[1];
			m_pointOldVertex[2] = pointOld[2];
			m_pointOldVertex[3] = pointOld[3];
		}
		else if (m_vecSizeLarge[m_nIndex] / (m_nCigarWidth*m_nCigarHeight) < 2.2&&m_pointVertex[0].x > 5 && m_pointVertex[1].x > 5)//只有两个则从后往前推
		{
			RectToPointLToR(m_pointOldVertex);//从左开始推第一个
		}
		else
		{
			//m_pointVertex[2] = pointOld[1];  //依据找出的分割区域从前往后推
			//m_pointVertex[3] = pointOld[0];
			//Case_3(m_pointOldVertex);
			//由于整个区域太大了，所以只分割一次，可能是以前出现过的烟，就循环找下一个了
			//新推出的第一条烟的最右端要小于上次推出的烟的最左端加一个差值，可以避免重复(大区域内重复)
			while (pointOld[2].x > (m_pointOldVertex[1].x + ERROR_VALUE) || pointOld[3].x >(m_pointOldVertex[0].x + ERROR_VALUE))
			{
				m_pointVertex[2] = pointOld[1];
				m_pointVertex[3] = pointOld[0];
				PointToPointRToL(pointOld);//从右向左推，上一次的0.1点作为推导的2.3点
			}
			m_pointOldVertex[0] = pointOld[0];
			m_pointOldVertex[1] = pointOld[1];
			m_pointOldVertex[2] = pointOld[2];
			m_pointOldVertex[3] = pointOld[3];
		}
	}
	else if (m_pointVertex[0].x > 5 && m_pointVertex[1].x > 5) //区域右端超出了区域，左端在区域内，从左到右推
	{
		RectToPointLToR(pointOld);//从左向右推，从外接矩形0.1点推导
		//新推出的第一条烟的最右端要小于上次推出的烟的最左端减一个差值，可以避免重复(大区域内重复)
		while (pointOld[2].x < (m_pointOldVertex[1].x - ERROR_VALUE) && pointOld[3].x<(m_pointOldVertex[0].x - ERROR_VALUE) && (m_nAreaWidth - std::max(pointOld[3].x, pointOld[2].x))>m_nCigarWidth)
		{
			m_pointVertex[0] = pointOld[3];
			m_pointVertex[1] = pointOld[2];
			cv::Point pointOldTemp[4];
			PointToPointLToR(pointOldTemp);//从左向右推，上次的2.3点作为推倒的0.1点
			if (pointOldTemp[2].x < (m_pointOldVertex[1].x + ERROR_VALUE) && pointOldTemp[3].x < (m_pointOldVertex[0].x + ERROR_VALUE))
			{
				pointOld[0] = pointOldTemp[0];
				pointOld[1] = pointOldTemp[1];
				pointOld[2] = pointOldTemp[2];
				pointOld[3] = pointOldTemp[3];
			}
			else break; //直到最右边的最右测点超出范围就结束
		}
		m_pointOldVertex[0] = pointOld[0];
		m_pointOldVertex[1] = pointOld[1];
		m_pointOldVertex[2] = pointOld[2];
		m_pointOldVertex[3] = pointOld[3];
	}
	else//右端超出了区域，左端也不在区域内,拥堵情况
	{
		if (m_pointOldVertex[1].x < m_nAreaWidth)
			m_pointVertex[2] = m_pointOldVertex[1];  //使用上一个往后推
		else
		{
			m_pointVertex[2] = m_pointOldVertex[1];  //有待推敲
			m_pointVertex[2].x = m_nAreaWidth;
		}
		if (m_pointOldVertex[0].x < m_nAreaWidth)
			m_pointVertex[3] = m_pointOldVertex[0];
		else
		{
			m_pointVertex[3] = m_pointOldVertex[0];
			m_pointVertex[3].x = m_nAreaWidth;
		}
		PointToPointRToL(m_pointOldVertex);  //依据上一次找出的分割区域(0.1点)从右往左推
	}
}

void Detector::LargeAreaSegBack()
{
	cv::Point pointOld[4];
	if (m_pointVertex[0].x>5 && m_pointVertex[1].x>5)////区域左端未超出区域
	{
		RectToPointLToR(pointOld);  //从左开始推的第一个
		if (pointOld[1].x>(m_pointOldVertex[2].x - ERROR_VALUE) && pointOld[0].x>(m_pointOldVertex[3].x - ERROR_VALUE))
		{
			m_pointOldVertex[0] = pointOld[0];
			m_pointOldVertex[1] = pointOld[1];
			m_pointOldVertex[2] = pointOld[2];
			m_pointOldVertex[3] = pointOld[3];
		}
		else if (m_vecSizeLarge[m_nIndex] / (m_nCigarWidth*m_nCigarHeight)<2.3&&m_pointVertex[2].x<(m_nAreaWidth - 5) && m_pointVertex[3].x<(m_nAreaWidth - 5))//只有两个则从后往前推
		{
			RectToPointRToL(m_pointOldVertex);  //从右往左推
		}
		else
		{
			while (pointOld[1].x < (m_pointOldVertex[2].x - ERROR_VALUE) && pointOld[0].x < (m_pointOldVertex[3].x - ERROR_VALUE))
			{
				m_pointVertex[0] = pointOld[3]; //从左往右推
				m_pointVertex[1] = pointOld[2]; 
				PointToPointLToR(pointOld);//从左向右推，上一次的2.3点作为推导的0.1点
			}
			m_pointOldVertex[0] = pointOld[0];
			m_pointOldVertex[1] = pointOld[1];
			m_pointOldVertex[2] = pointOld[2];
			m_pointOldVertex[3] = pointOld[3];
		}
	}
	else if (m_pointVertex[3].x<(m_nAreaWidth - 5) && m_pointVertex[2].x<(m_nAreaWidth - 5))//区域右端未超出区域
	{
		RectToPointRToL(pointOld); // 从右向左推第一个，从外接矩形2.3点推导
		while (pointOld[1].x>(m_pointOldVertex[2].x + ERROR_VALUE) && pointOld[0].x>(m_pointOldVertex[3].x + ERROR_VALUE) && std::max(pointOld[0].x, pointOld[1].x)>m_nCigarWidth)
		{
			m_pointVertex[3] = pointOld[0];
			m_pointVertex[2] = pointOld[1];
			cv::Point pointOldTemp[4];
			PointToPointRToL(pointOldTemp);//从右向左推，上次的0.1点作为推倒的2.3点
			if (pointOldTemp[1].x > (m_pointOldVertex[2].x - ERROR_VALUE) && pointOldTemp[0].x > (m_pointOldVertex[3].x - ERROR_VALUE))
			{
				pointOld[0] = pointOldTemp[0];
				pointOld[1] = pointOldTemp[1];
				pointOld[2] = pointOldTemp[2];
				pointOld[3] = pointOldTemp[3];
			}	
			else break;//直到最左边的最左测点超出范围就结束
		}
		m_pointOldVertex[0] = pointOld[0];
		m_pointOldVertex[1] = pointOld[1];
		m_pointOldVertex[2] = pointOld[2];
		m_pointOldVertex[3] = pointOld[3];
	}
	else //右端超出了区域，左端也不在区域内, 拥堵情况
	{
		if (m_pointOldVertex[2].x>1)
			m_pointVertex[1] = m_pointOldVertex[2];  //使用上一个往后推
		else
		{
			m_pointVertex[1] = m_pointOldVertex[2];  //有待推敲
			m_pointVertex[1].x = 1;
		}
		if (m_pointOldVertex[3].x>1)
			m_pointVertex[0] = m_pointOldVertex[3];
		else
		{
			m_pointVertex[0] = m_pointOldVertex[3];
			m_pointVertex[0].x = 1;
		}
		PointToPointLToR(m_pointOldVertex);//依据上一次找出的分割区域(2.3点)从左往右推
	}
}

void Detector::RectToPointRToL(cv::Point* pointOld) // 从右向左推，从外接矩形2.3点推导
{
	m_nFirst = FindClose(m_pointVertex[2]);
	m_nSecond = FindClose(m_pointVertex[3]);
	pointOld[2] = m_vecPointAcon[m_nFirst];
	pointOld[3] = m_vecPointAcon[m_nSecond];
 	pointOld[0] = pointOld[3];
 	pointOld[1] = pointOld[2];
	pointOld[0].x = std::max(pointOld[0].x - m_nCigarWidth, 1);
	pointOld[1].x = std::max(pointOld[1].x - m_nCigarWidth, 1); 
	int nHeightDiff = (fabs(pointOld[2].x - pointOld[3].x) / (fabs(pointOld[2].y - pointOld[3].y)+0.000001))*m_nCigarWidth;
	if (pointOld[2].x > pointOld[3].x) {	pointOld[0].y = std::max(pointOld[0].y - nHeightDiff, 1); pointOld[1].y = std::max(pointOld[1].y - nHeightDiff, 1);}
	if (pointOld[2].x < pointOld[3].x) { pointOld[0].y = std::min(pointOld[0].y + nHeightDiff, m_nRowNumber); pointOld[1].y = std::min(pointOld[1].y + nHeightDiff, m_nRowNumber);}
	Update_2(pointOld[0]);
	Update_2(pointOld[1]);
}

void Detector::RectToPointLToR(cv::Point* pointOld)//从左向右推，从外接矩形0.1点推导
{
	m_nFirst = FindClose(m_pointVertex[0]);
	m_nSecond = FindClose(m_pointVertex[1]);
	pointOld[0] = m_vecPointAcon[m_nFirst];
	pointOld[1] = m_vecPointAcon[m_nSecond];
 	pointOld[2] = pointOld[1];
 	pointOld[3] = pointOld[0];
	pointOld[2].x = std::min(pointOld[2].x + m_nCigarWidth, m_nAreaWidth);
	pointOld[3].x = std::min(pointOld[3].x + m_nCigarWidth, m_nAreaWidth);
	int nHeightDiff = (fabs(pointOld[0].x - pointOld[1].x) / (fabs(pointOld[0].y - pointOld[1].y) + 0.000001))*m_nCigarWidth;
	if (pointOld[0].x > pointOld[1].x) { pointOld[2].y = std::max(pointOld[2].y - nHeightDiff, 1); pointOld[3].y = std::max(pointOld[3].y - nHeightDiff, 1); }
	if (pointOld[0].x < pointOld[1].x) { pointOld[2].y = std::min(pointOld[2].y + nHeightDiff, m_nRowNumber); pointOld[3].y = std::min(pointOld[3].y + nHeightDiff, m_nRowNumber); }
	Update_2(pointOld[2]);
	Update_2(pointOld[3]);
}

void Detector::PointToPointRToL(cv::Point* pointOld)//从右向左推，2.3点(前一个0.1点)推导0.1点
{
	pointOld[2] = Update_1(m_pointVertex[2]);
	pointOld[3] = Update_1(m_pointVertex[3]);
	pointOld[0] = pointOld[3];
	pointOld[1] = pointOld[2];
	pointOld[0].x = std::max(pointOld[3].x - m_nCigarWidth, 1);
	pointOld[1].x = std::max(pointOld[2].x - m_nCigarWidth, 1);
	int nHeightDiff = (fabs(pointOld[2].x - pointOld[3].x) / (fabs(pointOld[2].y - pointOld[3].y) + 0.000001))*m_nCigarWidth;
	if (pointOld[2].x > pointOld[3].x) { pointOld[0].y = std::max(pointOld[0].y - nHeightDiff, 1); pointOld[1].y = std::max(pointOld[1].y - nHeightDiff, 1); }
	if (pointOld[2].x < pointOld[3].x) { pointOld[0].y = std::min(pointOld[0].y + nHeightDiff, m_nRowNumber); pointOld[1].y = std::min(pointOld[1].y + nHeightDiff, m_nRowNumber); }
	Update_2(pointOld[0]);
	Update_2(pointOld[1]);
}

void Detector::PointToPointLToR(cv::Point* pointOld)//从左向右推，0.1点推2.3点
{
	pointOld[0] = Update_1(m_pointVertex[0]);
	pointOld[1] = Update_1(m_pointVertex[1]);
	pointOld[2] = pointOld[1];
	pointOld[3] = pointOld[0];
	pointOld[2].x = std::min(pointOld[1].x + m_nCigarWidth, m_nAreaWidth);
	pointOld[3].x = std::min(pointOld[0].x + m_nCigarWidth, m_nAreaWidth);
 	int nHeightDiff = (fabs(pointOld[0].x - pointOld[1].x) / (fabs(pointOld[0].y - pointOld[1].y) + 0.000001))*m_nCigarWidth;
 	if (pointOld[0].x > pointOld[1].x) { pointOld[2].y = std::max(pointOld[2].y - nHeightDiff, 1); pointOld[3].y = std::max(pointOld[3].y - nHeightDiff, 1); }
 	if (pointOld[0].x < pointOld[1].x) { pointOld[2].y = std::min(pointOld[2].y + nHeightDiff, m_nRowNumber); pointOld[3].y = std::min(pointOld[3].y + nHeightDiff, m_nRowNumber); }
	Update_2(pointOld[2]);
	Update_2(pointOld[3]);
}

cv::Point Detector::Update_1(cv::Point pointValue)
{
	FindClose(pointValue, m_nFirst, m_nSecond);
	//if (abs(m_vecPointAcon[m_nSecond].x - pointValue.x)<25 && abs(m_vecPointAcon[m_nSecond].y - pointValue.y)<200)
	//改了，正向运动的时候，新的应该在原来的左边，所以约束范围改小了
	if ((m_vecPointAcon[m_nSecond].x - pointValue.x)<10 && (m_vecPointAcon[m_nSecond].x - pointValue.x)>-25 && abs(m_vecPointAcon[m_nSecond].y - pointValue.y)<200)
	{
		if (m_nSecond>0 && m_nSecond<(m_vecPointAcon.size() - 1))
		{
			int y_x1 = abs(m_vecPointAcon[m_nSecond].y - m_vecPointAcon[m_nSecond - 1].y) / (abs(m_vecPointAcon[m_nSecond].x - m_vecPointAcon[m_nSecond - 1].x) + 1);
			int y_x2 = abs(m_vecPointAcon[m_nSecond].y - m_vecPointAcon[m_nSecond + 1].y) / (abs(m_vecPointAcon[m_nSecond].x - m_vecPointAcon[m_nSecond + 1].x) + 1);
			if (y_x1<2 || y_x2<2)
				return m_vecPointAcon[m_nSecond];
		}
	}
	//这个判断内整个都是增加的
	if (2 == abs(int(m_nSecond - m_nFirst)) && abs(m_vecPointAcon[m_nFirst].x - pointValue.x)<25 && abs(m_vecPointAcon[m_nSecond].y - pointValue.y)<200)
	{
		int temp_point = (m_nSecond + m_nFirst) / 2;
		if (abs(m_vecPointAcon[temp_point].y - m_vecPointAcon[m_nSecond].y)>0.6*m_nCigarHeight&&abs(m_vecPointAcon[temp_point].y - m_vecPointAcon[m_nFirst].y)>0.6*m_nCigarHeight)
			return m_vecPointAcon[m_nSecond];
	}

	if (abs(m_vecPointAcon[m_nFirst].x - pointValue.x)<25 && abs(m_vecPointAcon[m_nFirst].y - pointValue.y)<200)
	{

		if (m_nFirst>0 && m_nFirst<(m_vecPointAcon.size() - 1))
		{
			int y_x1 = abs(m_vecPointAcon[m_nFirst].y - m_vecPointAcon[m_nFirst - 1].y) / (abs(m_vecPointAcon[m_nFirst].x - m_vecPointAcon[m_nFirst - 1].x) + 1);
			int y_x2 = abs(m_vecPointAcon[m_nFirst].y - m_vecPointAcon[m_nFirst + 1].y) / (abs(m_vecPointAcon[m_nFirst].x - m_vecPointAcon[m_nFirst + 1].x) + 1);
			if (y_x1<2 || y_x2<2)
				return m_vecPointAcon[m_nFirst];
		}
	}
	return pointValue;
}

void Detector::Update_2(cv::Point& pointValue)
{
	m_nFirst = FindClose(pointValue);
	if (abs(m_vecPointAcon[m_nFirst].x - pointValue.x)<30 && abs(m_vecPointAcon[m_nFirst].y - pointValue.y)<200)
	{
		if (m_nFirst>0 && m_nFirst<(m_vecPointAcon.size() - 1))
		{
			int y_x2 = abs(m_vecPointAcon[m_nFirst].y - m_vecPointAcon[m_nFirst + 1].y) / (abs(m_vecPointAcon[m_nFirst].x - m_vecPointAcon[m_nFirst + 1].x) + 1);
			int y_x1 = abs(m_vecPointAcon[m_nFirst].y - m_vecPointAcon[m_nFirst - 1].y) / (abs(m_vecPointAcon[m_nFirst].x - m_vecPointAcon[m_nFirst - 1].x) + 1);
			y_x2 = abs(m_vecPointAcon[m_nFirst].y - m_vecPointAcon[m_nFirst + 1].y) / (abs(m_vecPointAcon[m_nFirst].x - m_vecPointAcon[m_nFirst + 1].x) + 1);
			if (y_x1<2 || y_x2<2)
				pointValue = m_vecPointAcon[m_nFirst];
		}
	}
}

int Detector::FindClose(cv::Point pointValue)
{
	int nClose;
	int nTemp;
	int min_diff = INT_MAX;
	for (size_t i = 0; i<m_vecPointAcon.size(); i++)
	{
		nTemp = abs(pointValue.x - m_vecPointAcon[i].x) + abs(pointValue.y - m_vecPointAcon[i].y);
		if (nTemp<min_diff)
		{
			min_diff = nTemp;
			nClose = i;
		}
	}
	return nClose;
}

void Detector::FindClose(cv::Point pointValue, int& sizeFir, int& sizeSec)
{
	int nTemp;
	int min_diff = INT_MAX;
	int min_sec = INT_MAX;
	sizeFir = 0;
	int nSize = (int)m_vecPointAcon.size();
	for (int i = 0; i<nSize; i++)
	{
		nTemp = abs(pointValue.x - m_vecPointAcon[i].x) + abs(m_vecPointAcon[i].y - pointValue.y);
		if (nTemp<min_diff)
		{
			min_sec = min_diff;
			sizeSec = sizeFir;
			min_diff = nTemp;
			sizeFir = i;
		}
		else if (nTemp<min_sec)
		{
			min_sec = nTemp;
			sizeSec = i;
		}
	}
}

void Detector::RectToPoint(cv::Point* pointValue, cv::Rect rectValue)
{
	pointValue[0] = cv::Point(rectValue.x, rectValue.y + rectValue.height);
	pointValue[1] = cv::Point(rectValue.x, rectValue.y);
	pointValue[2] = cv::Point(rectValue.x + rectValue.width, rectValue.y);
	pointValue[3] = cv::Point(rectValue.x + rectValue.width, rectValue.y + rectValue.height);
}

void Detector::PointToRect()
{
	if (m_pointOldVertex[0].x<1)
		m_pointOldVertex[0].x = 1;
	if (m_pointOldVertex[1].x<1)
		m_pointOldVertex[1].x = 1;
	if (m_pointOldVertex[2].x>m_nAreaWidth-2)
		m_pointOldVertex[2].x = m_nAreaWidth-1;
	if (m_pointOldVertex[3].x>m_nAreaWidth-2)
		m_pointOldVertex[3].x = m_nAreaWidth-1;

	if (m_pointOldVertex[1].y<1)
		m_pointOldVertex[1].y = 1;
	if (m_pointOldVertex[2].y<1)
		m_pointOldVertex[2].y = 1;
	if (m_pointOldVertex[0].y>m_nAreaHeight-2)
		m_pointOldVertex[0].y = m_nAreaHeight-1;
	if (m_pointOldVertex[3].y>m_nAreaHeight-2)
		m_pointOldVertex[3].y = m_nAreaHeight-1;

	m_rectResult.x = std::min(m_pointOldVertex[0].x, m_pointOldVertex[1].x);
	m_rectResult.y = std::min(m_pointOldVertex[1].y, m_pointOldVertex[2].y);
	m_rectResult.width = std::max(m_pointOldVertex[2].x, m_pointOldVertex[3].x) - m_rectResult.x;
	m_rectResult.height = std::max(m_pointOldVertex[0].y, m_pointOldVertex[3].y) - m_rectResult.y;
}

void Detector::UpToOldPoint()
{
	for (int i = 0; i<4; i++)
		m_pointOldVertex[i].x = m_pointOldVertex[i].x + SPEED_VALUE;
}

void Detector::DownToOldPoint()
{
	for (int i = 0; i<4; i++)
		m_pointOldVertex[i].x = m_pointOldVertex[i].x - SPEED_VALUE;
}

void Detector::FindMaxXArea()   //找出X坐标最大的的Rect
{
	int max_X = m_vecRectLarge[0].x;  
	m_nIndex = 0;
	for (size_t i = 0; i<m_vecRectLarge.size(); i++)
	if (m_vecRectLarge[i].x>max_X)
	{
		max_X = m_vecRectLarge[i].x;
		m_nIndex = i;
	}
}

void Detector::FindMinXArea()
{
	int max_X = m_vecRectLarge[0].x;
	m_nIndex = 0;
	for (size_t i = 0; i<m_vecRectLarge.size(); i++)
	if (m_vecRectLarge[i].x<max_X)
	{
		max_X = m_vecRectLarge[i].x;
		m_nIndex = i;
	}
}

void Detector::FindMaxOfSmallSizes()
{
	m_dMaxSmallSizes = 0;
	for (size_t i = 0; i<m_vecSizeSmall.size(); i++)
		if (m_vecSizeSmall[i]>m_dMaxSmallSizes)
			m_dMaxSmallSizes = m_vecSizeSmall[i];  //小区域的最大值
}

bool Detector::SortRect(const cv::Rect &Rect1, const cv::Rect &Rect2)
{
	if ((Rect1.x + Rect1.width)<(Rect2.x + Rect2.width))
		return true;
	else
		return false;
}

void Detector::DeleteRect()
{
	for (int i = (int)m_vecRectSmall.size() - 1; i >= 0; i--)
	{
		if (m_vecRectSmall[i].y<m_nAreaHeight / 4)
			if (m_pointOldVertex[1].x - (m_vecRectSmall[i].x + m_vecRectSmall[i].width) + 20<0)
			{
				m_vecRectSmall.erase(m_vecRectSmall.begin() + i);
				continue;
			}
			else 
				return;
		if (m_vecRectSmall[i].y + m_vecRectSmall[i].height>3 * m_nAreaHeight / 4)
			if (m_pointOldVertex[0].x - (m_vecRectSmall[i].x + m_vecRectSmall[i].width) + 20<0)
			{
				m_vecRectSmall.erase(m_vecRectSmall.begin() + i);
				continue;
			}
			else 
				return;
		if ((m_pointOldVertex[0].x + m_pointOldVertex[1].x) / 2 - (m_vecRectSmall[i].x + m_vecRectSmall[i].width) + 20<0)
			m_vecRectSmall.erase(m_vecRectSmall.begin() + i);
		else 
			return;
	}
	return;
}

bool Detector::GatherAreaFirst()
{
	sort(m_vecRectSmall.begin(), m_vecRectSmall.end(), SortRect);
	DeleteRect();
	FilterSmallRect();
	if (m_vecRectSmall.size()>0 && m_nSmallAreaNum>1 && m_vecRectSmall[m_vecRectSmall.size() - 1].x + m_vecRectSmall[m_vecRectSmall.size() - 1].width>2.5*m_nCigarWidth)
	{
		return ComputeVertex();
	}
	else
		return false;
}

bool Detector::GatherAreaSecond()
{
	m_vecRectSmall.insert(m_vecRectSmall.end(), m_vecRectLarge.begin(), m_vecRectLarge.end());
	sort(m_vecRectSmall.begin(), m_vecRectSmall.end(), SortRect);
	DeleteRect();
	FilterSmallRect();
	if (m_vecRectSmall.size()>0 && m_nSmallAreaNum>2 && m_vecRectSmall[m_vecRectSmall.size() - 1].x + m_vecRectSmall[m_vecRectSmall.size() - 1].width>2.5*m_nCigarWidth)
	{
		return ComputeVertex();
	}
	else 
		return false;
}

bool Detector::ComputeVertex()
{
	std::vector<int> vecXt(4);
	vecXt[0] = m_nAreaWidth;
	vecXt[1] = m_nAreaWidth;
	std::vector<int> vecXf(4);
	cv::Rect rectSum;
	for (int i = (int)m_vecRectSmall.size() - 1; i >= 0; i--)
	{
		if (m_vecRectSmall[i].y<m_nAreaHeight / 4 && m_vecRectSmall[i].x<vecXt[1])
			vecXt[1] = m_vecRectSmall[i].x;
		if (m_vecRectSmall[i].y + m_vecRectSmall[i].height>3 * m_nAreaHeight / 4 && m_vecRectSmall[i].x<vecXt[0])
			vecXt[0] = m_vecRectSmall[i].x;
		if (m_vecRectSmall[i].y<m_nAreaHeight / 4 && m_vecRectSmall[i].x + m_vecRectSmall[i].width>vecXt[2])
			vecXt[2] = m_vecRectSmall[i].x + m_vecRectSmall[i].width;
		if (m_vecRectSmall[i].y + m_vecRectSmall[i].height>3 * m_nAreaHeight / 4 && m_vecRectSmall[i].x + m_vecRectSmall[i].width>vecXt[3])
			vecXt[3] = m_vecRectSmall[i].x + m_vecRectSmall[i].width;
		if (i == m_vecRectSmall.size() - 1)
			rectSum = m_vecRectSmall[i];
		else
			rectSum = rectSum | m_vecRectSmall[i];
		if (rectSum.width>2.5*m_nCigarWidth&&rectSum.height>2.5*m_nCigarWidth || (vecXt[2] - vecXt[1])>1.2*m_nCigarWidth || (vecXt[3] - vecXt[0]>1.2*m_nCigarWidth))
		{
			if (m_rectResult.height*m_rectResult.width<0.4*m_nCigarWidth*m_nCigarHeight)
				return false;
			RectToPoint(m_pointOldVertex, m_rectResult);
			if (m_rectResult.y<m_nAreaHeight / 4)
			{
				if (vecXf[2] - vecXf[1]<0.8*m_nCigarWidth)
				if (vecXf[1] - m_pointOldVertex[1].x>m_pointOldVertex[2].x - vecXf[2])
				{
					m_pointOldVertex[2].x = vecXf[2];
					m_pointOldVertex[1].x = std::max(m_pointOldVertex[2].x - m_nCigarWidth, 1);
				}
				else
				{
					m_pointOldVertex[1].x = vecXf[1];
					m_pointOldVertex[2].x = std::min(m_pointOldVertex[1].x + m_nCigarWidth, m_nAreaWidth);
				}
				else
				{
					m_pointOldVertex[1].x = vecXf[1];
					m_pointOldVertex[2].x = vecXf[2];
				}
			}
			if (m_rectResult.y + m_rectResult.height>3 * m_nAreaHeight / 4)
			{
				if (vecXf[3] - vecXf[0]<0.8*m_nCigarWidth)
				if (vecXf[0] - m_pointOldVertex[0].x>m_pointOldVertex[3].x - vecXf[3])
				{
					m_pointOldVertex[3].x = vecXf[3];
					m_pointOldVertex[0].x = std::max(m_pointOldVertex[3].x - m_nCigarWidth, 1);
				}
				else
				{
					m_pointOldVertex[0].x = vecXf[0];
					m_pointOldVertex[3].x = std::min(m_pointOldVertex[0].x + m_nCigarWidth, m_nAreaWidth);
				}
				else
				{
					m_pointOldVertex[0].x = vecXf[0];
					m_pointOldVertex[3].x = vecXf[3];
				}
			}
			return true;
		}
		else
		{
			vecXf = vecXt;
			m_rectResult = rectSum;
		}
	}
	//循环完所有的区域，自然退出
	if (m_rectResult.width*m_rectResult.height<0.5*m_nCigarWidth*m_nCigarHeight)
		return false;
	RectToPoint(m_pointOldVertex, m_rectResult);
	if (m_rectResult.x<m_nAreaHeight / 4)
	{
		m_pointOldVertex[1].x = vecXf[1];
		m_pointOldVertex[2].x = vecXf[2];
	}
	if (m_rectResult.x>3 * m_nAreaHeight / 4)
	{
		m_pointOldVertex[0].x = vecXf[0];
		m_pointOldVertex[3].x = vecXf[3];
	}
	return true;
}

void Detector::AffineTransform(cv::Mat& matImage, cv::Rect rectValue, cv::Point* pPointInput)
{
	cv::Point2f pointSrc[3];
	cv::Point2f pointDst[3];
	cv::Mat matWarp(2, 3, CV_32FC1);
	cv::Mat dstImage = cv::Mat::zeros(matImage.rows, matImage.cols, matImage.type());
	pointSrc[1].x = (float)pPointInput[0].x - rectValue.x;
	pointSrc[1].y = (float)pPointInput[0].y - rectValue.y;
	pointSrc[0].x = (float)pPointInput[1].x - rectValue.x;
	pointSrc[0].y = (float)pPointInput[1].y - rectValue.y;
	pointSrc[2].x = (float)pPointInput[2].x - rectValue.x;
	pointSrc[2].y = (float)pPointInput[2].y - rectValue.y;
	pointDst[1] = cv::Point2f(1, (float)matImage.rows - 1);
	pointDst[0] = cv::Point2f(1, 1);
	pointDst[2] = cv::Point2f((float)matImage.cols - 1, 1);
	matWarp = getAffineTransform(pointSrc, pointDst);
	warpAffine(matImage, dstImage, matWarp, dstImage.size());
	cv::Size sizeOut(170, 525);
	resize(dstImage, m_matSegment, sizeOut);
}

