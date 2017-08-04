#ifndef detector_wrapper_h_
#define detector_wrapper_h_

#include "Detector.h"
#include "opencv2\opencv.hpp"
#include <memory>

class DetectorWrapper
{
public:
	DetectorWrapper();
	~DetectorWrapper();

	bool detect(cv::Mat img);
	cv::Mat get_segment_image();
	cv::Point* get_result_point();

private:
	std::shared_ptr<Detector> detector_;
};


#endif // !detector_wrapper_h_
