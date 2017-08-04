#ifndef cigarette_algorithm_thread_h_
#define cigarette_algorithm_thread_h_

#include "Common.h"
#include "Detector.h"
#include "mqueue.h"
#include "Recognizer.h"
#include "Session.h"
#include "SmartPlatform.pb.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread/thread.hpp"
#include <memory>

class CigaretteAlgorithmThread
{
public:
	explicit CigaretteAlgorithmThread(std::string thread_name);
	~CigaretteAlgorithmThread();

	int start();
	void execute(const std::shared_ptr<MessageWithSession> &message);
	void stop();

private:
	void process();
	void process_invalid_image_data(boost::shared_ptr<Session> session);
	void process_detect_success(std::shared_ptr<SmartPlatform::Message> message, boost::shared_ptr<Session> session, int db_id);
	void process_detect_fail(std::shared_ptr<SmartPlatform::Message> message, boost::shared_ptr<Session> session);
	void change_int_to_bytes(int target, unsigned char *buffer);

private:
	bool is_run_;
	boost::thread thread_;
	std::string thread_name_;
	MessageQueuePtr message_queue_;
	std::shared_ptr<Detector> detector_;
	std::shared_ptr<Recognizer> recognizer_;
};

#endif //cigarette_algorithm_thread_h_


