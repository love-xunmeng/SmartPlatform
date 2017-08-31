#include "CameraObscureAlgorithm.h"
#include "CameraObscured.h"
#include "IAlgorithm.h"
#include "Util.h"
#ifdef linux
#include "FreeImage.h"
#endif // linux
#include "ImageStone.h"
#include "rapidjson/document.h"     
#include "rapidjson/prettywriter.h" 
#include "opencv2/opencv.hpp"
#include <string>

using namespace rapidjson;
using namespace cv;
using namespace std;

CameraObscureAlgorithm::CameraObscureAlgorithm()
	:algorithm_(new CameraAbnormalDetector())
{
}


CameraObscureAlgorithm::~CameraObscureAlgorithm()
{
}

std::string CameraObscureAlgorithm::process(std::string json_data){
	//cout << "json_data=" << json_data << endl;

	Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
	if (document.Parse(json_data.c_str()).HasParseError()) {
		return process_server_error();
	}

	int image_width = document["image_width"].GetInt();
	int image_height = document["image_height"].GetInt();
	int channels = document["image_channels"].GetInt();
	string image_type = document["image_type"].GetString();
	string str_base64_image_data = document["image_data"].GetString();
	string str_image_data;
	Util::Base64Decode(str_base64_image_data, &str_image_data);
	FCObjImage img;
	img.Load(str_image_data.c_str(), str_image_data.length(), IMG_JPG);
	cv::Mat img_src(image_height, image_width, CV_8UC3, (void*)(img.GetMemStart()));
	bool is_obscured = algorithm_->Check_Camera_Obscured(img_src, 128);
	imwrite("e:\\tttttt.png", img_src);

	
	Document doc;
	doc.SetObject();
	Document::AllocatorType &allocator = doc.GetAllocator();
	doc.AddMember("status_code", 200, allocator);
	doc.AddMember("is_obscured", is_obscured, allocator);
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}

std::string CameraObscureAlgorithm::process_server_error() {
	Document doc;
	doc.SetObject();
	Document::AllocatorType &allocator = doc.GetAllocator();
	doc.AddMember("status_code", 400, allocator);
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}