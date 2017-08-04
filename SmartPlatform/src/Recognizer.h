#ifndef recognizer_h_
#define recognizer_h_

#include "opencv2/opencv.hpp"

class Recognizer
{
public:
	Recognizer();
	~Recognizer();

	int recognize(cv::Mat img);

private:
	int init();
	void destroy();
	
};

#endif // recognizer_h_


