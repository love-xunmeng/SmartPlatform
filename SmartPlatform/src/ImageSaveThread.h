#ifndef image_save_thread_h_
#define image_save_thread_h_

#include "mqueue.h"
#include "opencv2\opencv.hpp"
#include "boost/thread/thread.hpp"
#include <string>
#include <queue>

class ImageSaveThread
{
public:
	ImageSaveThread(std::string root_dir);
	~ImageSaveThread();

	static ImageSaveThread *instance();

	int start();
	void stop();
	void save(cv::Mat img);

private:
	int init();
	void create_root_dir_if_it_does_not_exist();
	void run();

private:
	static ImageSaveThread *instance_;

private:
	bool is_run_ = false;
	std::string root_dir_ = "";
	boost::thread thread_;
	//MQuque<cv::Mat> image_list_;
	std::queue<cv::Mat> image_list_;
	std::string str_last_hour_ = "";
	long long last_time_ = 0;
};

#endif // !image_save_thread_h_
