#include "ImageSaveThread.h"
#include "Config.h"
#include "MLog.h"
#include "mqueue.h"
#include "opencv2\opencv.hpp"
#include "boost\date_time\gregorian\gregorian.hpp"
#include "boost\date_time\posix_time\posix_time.hpp"
#include "boost/function/function0.hpp"
#include "boost\filesystem.hpp"
#include <fstream>
#include <sstream>
#include <string>

using namespace cv;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::filesystem;
using namespace boost;
using namespace std;

ImageSaveThread* ImageSaveThread::instance_ = NULL;

ImageSaveThread::ImageSaveThread(std::string root_dir)
	:root_dir_(root_dir)
{
	init();
}


ImageSaveThread::~ImageSaveThread()
{
}

ImageSaveThread* ImageSaveThread::instance() {
	if (NULL == instance_) {
		instance_ = new ImageSaveThread(Config::instance()->image_save_root_dir());
	}
	return instance_;
}

int ImageSaveThread::init() {
	create_root_dir_if_it_does_not_exist();
	return 0;
}

void ImageSaveThread::create_root_dir_if_it_does_not_exist(){
	namespace bf = boost::filesystem;
	if (!bf::exists(root_dir_)) {
		bf::create_directories(root_dir_);
	}
}

int ImageSaveThread::start() {
	is_run_ = true;
	boost::function0<void> f = boost::bind(&ImageSaveThread::run, this);
	thread_ = boost::thread(f);
	return 0;
}

void ImageSaveThread::stop() {
	if (is_run_)
	{
		is_run_ = false;
		if (thread_.joinable())
		{
			//如果pop被阻塞，又没有数据进来，应该会死锁的，应该设置超时
			thread_.join();
		}
	}
}

void ImageSaveThread::save(cv::Mat img) {
	if (image_list_.size() >= 1000) {
		return;
	}
	image_list_.push(img);
}

void ImageSaveThread::run() {
	//这是支线任务，有任何问题都不应该影响主线执行
	try {
		while (is_run_) {

			if (0 == image_list_.size()) {
				boost::this_thread::sleep(boost::posix_time::milliseconds(100));
				continue;
			}

			ptime now = microsec_clock::local_time();

			char buffer[256] = { 0 };
			sprintf(buffer, "%02d", now.time_of_day().hours());
			string str_date = to_iso_extended_string(now.date());
			string str_hour = string(buffer);
			if (str_hour != str_last_hour_) {
				create_directories(root_dir_ + "\\" + str_date + "\\" + str_hour);
				str_last_hour_ = str_hour;
			}
			
			ostringstream ss;			
			long long current_time = now.time_of_day().total_milliseconds();
			if (0 == current_time) {
				last_time_ = current_time;
			}
			else {
				if (current_time == last_time_) {
					current_time += 1;
				}
			}
			last_time_ = current_time;
			static int count = 100001;
			ss << root_dir_ << "\\" << str_date << "\\" << buffer << "\\" << current_time << "_" << count << ".bmp";
			++count;
			cout << ss.str() << endl;

			

			cv::Mat img = image_list_.front();
			image_list_.pop();
			//if (i == 2) {
			//	i = 0;
			cout << ss.str() << endl;
			if (img.empty()) {
				cerr << "img.empty()" << endl;
			}
				imwrite(ss.str(), img);
			//}
			//++i;
		}
	}
	catch (std::exception &e) {
		MLogger.error(e.what());
	}
	catch (...) {
		ostringstream ss;
		ss << "Unkonwn Error in ImageSaveThread, thread id is: " << thread_.get_id() << endl;
		MLogger.error(ss.str());
	}
}