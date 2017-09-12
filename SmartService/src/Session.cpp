#include "Session.h"
#include "DataItem.h"
#include "Util.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <string>
#include <time.h>
using boost::asio::ip::tcp;
using namespace std;

Session::Session(boost::asio::io_service& ioservice, boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>> queue)
	: socket_(ioservice)
	, queue_(queue)
	, data_length_bytes_(4)
	, BUFFER_SIZE(1024*1024)
	, DATA_SIZE(10 * 1024 * 1024)
{
	//申请内存应该判断的，内存不够的时候应该做相应的处理
	data_ = new char[DATA_SIZE];
	memset(data_, 0, DATA_SIZE);
	buffer_ = new char[BUFFER_SIZE];
	memset(buffer_, 0, BUFFER_SIZE);
	data_size_ = 0;
	last_read_data_length_time_ = 0;
	last_read_data_time_ = 0;
	offset_ = 0;
	is_start_read_ = false;
	pb_start_ = nullptr;

	//boost::asio::ip::tcp::no_delay noDelayOption(true);
	//socket_.set_option(noDelayOption);
}

Session::~Session()
{
	if (buffer_)
	{
		delete[]buffer_;
		buffer_ = nullptr;
	}
	if (data_) {
		delete[]data_;
		data_ = nullptr;
	}
}

void Session::start() {
	boost::asio::async_read(socket_,
		boost::asio::buffer(data_, data_length_bytes_),
		boost::bind(&Session::handle_read_data_length, shared_from_this(), boost::asio::placeholders::error));
}

void Session::handle_read_data_length(const boost::system::error_code &error) {
	if (!error)
	{
		data_size_ = Util::bytes_to_int((unsigned char*)data_);
		boost::asio::async_read(socket_,
			boost::asio::buffer(data_, data_size_),
			boost::bind(&Session::handle_read_data, shared_from_this(), boost::asio::placeholders::error));
	}
	else {
		process_error_code(error);
	}
}

void Session::handle_read_data(const boost::system::error_code &error) {
	if (!error)
	{
		boost::shared_ptr<DataItem> camera_occlusion_data(new DataItem());
		camera_occlusion_data->json_data_ = string(data_, data_size_);
		camera_occlusion_data->session_ = shared_from_this();
		queue_->push(camera_occlusion_data);
		boost::asio::async_read(socket_,
			boost::asio::buffer(data_, data_length_bytes_),
			boost::bind(&Session::handle_read_data_length, shared_from_this(), boost::asio::placeholders::error));
	}
	else {
		process_error_code(error);
	}
}

void Session::write(std::string message) {
	unsigned char bytes[4] = { 0 };
	Util::int_to_byte(message.length(), bytes);
	try {
		static char tmp[1024];
		memset(tmp, 0, 1024);
		memcpy(tmp, bytes, 4);
		memcpy(tmp + 4, message.c_str(), message.length());
		boost::asio::write(socket_, boost::asio::buffer(tmp, 4 + message.length()));
	}
	catch(std::exception &e)
	{
		cerr << "Error: " << e.what() << endl;
	}
}

void Session::handle_write_data(const boost::system::error_code &error) {
	if (!error)
	{
	}
	else
	{
		static long i = 0;
		++i;
		cout << "write error count: " << i << endl;
		process_error_code(error);
	}
}

void Session::process_error_code(const boost::system::error_code &error) {
	if (error == boost::asio::error::eof) {
		//cout << "Remote client closed." << endl;
	}
	else if (error == boost::asio::error::bad_descriptor) {
		cerr << "boost::asio::error::bad_descriptor" << endl;
	}
	else if (error == boost::asio::error::operation_aborted) {
		cerr << "boost::asio::error::operation_aborted" << endl;
	}
	else if (error == boost::asio::error::connection_reset) {
		cerr << "error == boost::asio::error::connection_reset" << endl;
	}
	else {
		cerr << "read_handler error" << endl;
	}
}