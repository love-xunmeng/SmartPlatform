#ifndef session_h_
#define session_h_

#include "DataItem.h"
#include "mqueue.h"
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <memory>
#include <string>
using boost::asio::ip::tcp;

class Session : public boost::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_service& ioservice, boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>>);
	~Session();

	tcp::socket& socket()
	{
		return socket_;
	}

	void start();
	void write(std::string message);

private:
	void handle_read_data_length(const boost::system::error_code &error);
	void handle_read_data(const boost::system::error_code &error);
	void handle_write_data(const boost::system::error_code &error);
	void process_error_code(const boost::system::error_code &error);

private:
	const long BUFFER_SIZE;
	const long DATA_SIZE;
	
private:
	const int data_length_bytes_;
	tcp::socket socket_;
	char *buffer_;
	char *data_;
	long data_size_;
	long last_read_data_length_time_;
	long last_read_data_time_;
	long offset_;
	bool is_start_read_;
	char *pb_start_;
	boost::shared_ptr<MQuque<boost::shared_ptr<DataItem>>> queue_;
};

#endif // session_h_


