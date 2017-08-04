#include "Recognizer.h"
#include "Classification.h"
#include "MLog.h"
#include "opencv2\opencv.hpp"
#include <string>
#include <vector>
#include "Windows.h"
using namespace cv;
using namespace std;

Recognizer::Recognizer()
{
	init();
}


Recognizer::~Recognizer()
{
	destroy();
}

int Recognizer::init() {
	//应该从配置中读取这些值
	MLogger.info("Loading Caffe Model...");

	std::string model_file("./data/model/caffenet_finetune_deploy.prototxt");
	std::string trained_file("./data/model/caffenet_finetune_iter_20000.caffemodel");
	std::string mean_file("./data/model/train_mean.binaryproto");

	bool ret = InitialSource(model_file, trained_file, mean_file);
	if (!ret)
	{
		cerr << "InitialSource Error" << endl;
		exit(1);
		return -1;
	}
	MLogger.info("Finish loading Caffe Model...");
	return 0;
}

void Recognizer::destroy() {
	DestroySource();
}

int Recognizer::recognize(cv::Mat img) {
	//DWORD starttime = GetTickCount();
	//Mat img2 = imread("e:\\testDataResultBack\\mask_22.bmp");
	NetProcess(img);
	sPrediction* pResultFirst = Classify(img, 0, 88);
	sPrediction spFirst = pResultFirst[0];
	int prediect_id = spFirst.nClass; 
	//DWORD endtime = GetTickCount();
	float protability = spFirst.fProbability; 
	//double acttime = (double)(endtime - starttime);
	//cout << "Time: " << acttime << ", predict_id=" << 222222222 << endl;
	return prediect_id;
}