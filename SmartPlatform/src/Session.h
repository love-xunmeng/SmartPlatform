#ifndef session_h_
#define session_h_

#include "Common.h"
#include "RawData.h"
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <memory>
#include <string>
using boost::asio::ip::tcp;

class Session : public boost::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_service& ioservice, RawMessageDataQueuePtr raw_message_data_queue);
	~Session();

	tcp::socket& socket()
	{
		return socket_;
	}

	void start();
	//void write(void* data, int length);
	void write(std::string message);

private:
	void handler_for_async_read_some(const boost::system::error_code &error, std::size_t bytes_transferred);
	void handle_read_data_length(const boost::system::error_code &error);
	void handle_read_data(const boost::system::error_code &error);
	void handle_write_data(const boost::system::error_code &error);
	void process_error_code(const boost::system::error_code &error);
	void Session::change_int_to_bytes(int target, unsigned char *buffer);

private:
	const long BUFFER_SIZE;
	const long DATA_SIZE;
	
private:
	const int data_length_bytes_;
	tcp::socket socket_;
	char *buffer_;
	char *data_;
	RawMessageDataQueuePtr raw_message_data_queue_;
	long data_size_;
	long last_read_data_length_time_;
	long last_read_data_time_;
	long offset_;
	bool is_start_read_;
	char *pb_start_;
};

#endif // session_h_


