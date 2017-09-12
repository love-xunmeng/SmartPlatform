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
	try {
		Document document;
		if (document.Parse(json_data.c_str()).HasParseError()) {
			return process_server_error();
		}

		if (!document.IsObject()) {
			return process_server_error();
		}

		if(!document.HasMember("image_width")) return process_server_error();
		if (!document.HasMember("image_height")) return process_server_error();
		if (!document.HasMember("image_color_bits")) return process_server_error();
		if (!document.HasMember("image_type")) return process_server_error();
		if (!document.HasMember("image_data")) return process_server_error();

		const int image_width = document["image_width"].GetInt();
		const int image_height = document["image_height"].GetInt();
		const int image_color_bits = document["image_color_bits"].GetInt();
		const string image_type = document["image_type"].GetString();
		const string str_base64_image_data = document["image_data"].GetString();
		string str_image_data;
		try {
			Util::Base64Decode(str_base64_image_data, &str_image_data);
		}
		catch (std::exception &e) {
			cerr << __FILE__ << " " << __func__ << " " << __LINE__ << e.what() << endl;
			return process_server_error();
		}
		
		if (0 == str_image_data.size()) {
			return process_server_error();
		}
		
		FCObjImage img;
		IMAGE_TYPE fc_image_type;
		if ("jpg" == image_type) {
			fc_image_type = IMG_JPG;
		}
		else if("png" == image_type) {
			fc_image_type = IMG_PNG;
		}
		else if ("bmp" == image_type) {
			fc_image_type = IMG_BMP;
		}
		else {
			return process_server_error();
		}
		bool is_load_success = img.Load(str_image_data.c_str(), str_image_data.length(), fc_image_type);
		if (img.Width() != image_width || img.Height() != image_height || img.ColorBits() != image_color_bits || !is_load_success) {
			return process_server_error();
		}
		cv::Mat img_src(image_height, image_width, CV_8UC3, (void*)(img.GetMemStart()));
		bool is_obscured = algorithm_->Check_Camera_Obscured(img_src, 128);

		Document doc;
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		doc.AddMember("Status", 200, allocator);
		doc.AddMember("is_obscured", is_obscured, allocator);
		StringBuffer buffer;
		PrettyWriter<StringBuffer> writer(buffer);
		doc.Accept(writer);
		return buffer.GetString();
	}
	catch (std::exception &e) {
		cerr << __FILE__ << " " << __func__ << " " << __LINE__ << e.what() << endl;
		return process_server_error();
	}
}

std::string CameraObscureAlgorithm::process_server_error() {
	Document doc;
	doc.SetObject();
	Document::AllocatorType &allocator = doc.GetAllocator();
	doc.AddMember("Status", 400, allocator);
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}