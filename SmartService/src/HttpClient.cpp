#include "HttpClient.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;
using namespace std;

HttpClient::HttpClient()
{
}


HttpClient::~HttpClient()
{
}

int HttpClient::request(std::string host, int port, std::string request_path, std::string data) {
	try
	{
		boost::asio::io_service io_service;

		tcp::endpoint end_point(boost::asio::ip::address::from_string(host), port);

		// Try each endpoint until we successfully establish a connection.
		tcp::socket socket(io_service);
		socket.connect(end_point);

		// Form the request. We specify the "Connection: close" header so that the
		// server will close the socket after transmitting the response. This will
		// allow us to treat all data up until the EOF as the content.
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "POST " << request_path << " HTTP/1.0\r\n";
		request_stream << "Host: " << host << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		// Send the request.
		boost::asio::write(socket, request);

		// Read the response status line. The response streambuf will automatically
		// grow to accommodate the entire line. The growth may be limited by passing
		// a maximum size to the streambuf constructor.
		boost::asio::streambuf response;
		boost::asio::read_until(socket, response, "\r\n");

		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			std::cout << "Invalid response\n";
			return -1;
		}
		if (status_code != 200)
		{
			std::cout << "Response returned with status code " << status_code << "\n";
			return -1;
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::read_until(socket, response, "\r\n\r\n");

		// Process the response headers.
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
			std::cout << header << "\n";
		std::cout << "\n";

		cout << endl;
		cout << "Header Finish" << endl;
		cout << "Content: " << endl;

		// Write whatever content we already have to output.
		if (response.size() > 0)
			std::cout << &response << endl;

		// Read until EOF, writing data to output as we go.
		//boost::system::error_code error;
		//while (boost::asio::read(socket, response,
		//	boost::asio::transfer_at_least(1), error))
		//	std::cout << "callback: " << &response;
		//cout << "error: " << error << endl;
		//if (error != boost::asio::error::eof)
		//	throw boost::system::system_error(error);

		socket.close();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
	}

	return 0;
}