#include "Common.h"
#include "MLog.h"
#include "RawDataWithSession.h"
#include "Session.h"
#include "Util.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <time.h>
using boost::asio::ip::tcp;
using namespace std;

Session::Session(boost::asio::io_service& ioservice, RawMessageDataQueuePtr raw_message_data_queue)
	:socket_(ioservice)
	, raw_message_data_queue_(raw_message_data_queue)
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

void Session::handler_for_async_read_some(const boost::system::error_code &error, std::size_t bytes_transferred) {
	if (!error)
	{
		if (offset_ + bytes_transferred < DATA_SIZE)
		{
			memcpy(data_+ offset_, buffer_, bytes_transferred);
			offset_ += bytes_transferred;
		}
		else {
			long data_left = DATA_SIZE - offset_;
			memcpy(data_ + offset_, buffer_, data_left);
			long buffer_left = bytes_transferred - data_left;
			memcpy(data_, buffer_ + data_left, buffer_left);
		}
		if (is_start_read_) {
			is_start_read_ = false;
			long data_size_ = Util::bytes_to_int((unsigned char*)data_);
			cout << "data_size: " << data_size_ << endl;
			pb_start_ = pb_start_ + data_length_bytes_ * sizeof(char);
			if (bytes_transferred - data_length_bytes_ >= data_size_)
			{
				std::shared_ptr<RawDataWithSession> raw_data = std::shared_ptr<RawDataWithSession>(new RawDataWithSession());
				raw_data->raw_data_ = std::shared_ptr<RawData>(new RawData(pb_start_, data_size_));
				raw_data->session_ = shared_from_this();
				raw_message_data_queue_->push(raw_data);
			}
		}
		else {

		}

		socket_.async_read_some(boost::asio::buffer(data_, BUFFER_SIZE),
			boost::bind(&Session::handler_for_async_read_some,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		process_error_code(error);
	}
}

void Session::write(std::string message) {
	unsigned char bytes[4];
	change_int_to_bytes(message.length(), bytes);
	try {
		static char tmp[1024];
		memset(tmp, 0, 1024);
		memcpy(tmp, bytes, 4);
		memcpy(tmp + 4, message.c_str(), message.length());
		boost::asio::write(socket_, boost::asio::buffer(tmp, 4 + message.length()));
	}
	catch(std::exception &e)
	{
		MLogger.error(string("Socket write error: ") + string(e.what()));
	}
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
		std::shared_ptr<RawDataWithSession> raw_data = std::shared_ptr<RawDataWithSession>(new RawDataWithSession());
		raw_data->raw_data_ = std::shared_ptr<RawData>(new RawData(data_, data_size_));
		raw_data->session_ = shared_from_this();
		raw_message_data_queue_->push(raw_data);
		boost::asio::async_read(socket_,
			boost::asio::buffer(data_, data_length_bytes_),
			boost::bind(&Session::handle_read_data_length, shared_from_this(), boost::asio::placeholders::error));
	}
	else {
		process_error_code(error);
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
		cout << "Remote client closed." << endl;
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

void Session::change_int_to_bytes(int target, unsigned char *buffer) {
	memset(buffer, 0, sizeof(unsigned char) * 4);
	buffer[0] = (unsigned char)(0xff & target);
	buffer[1] = (unsigned char)((0xff00 & target) >> 8);
	buffer[2] = (unsigned char)((0xff0000 & target) >> 16);
	buffer[3] = (unsigned char)((0xff000000 & target) >> 24);
}