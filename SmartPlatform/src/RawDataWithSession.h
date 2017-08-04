#ifndef raw_data_with_session_h_
#define raw_data_with_session_h_

#include "RawData.h"
#include "RawDataWithSession.h"
#include "Session.h"
#include <memory>

class RawDataWithSession {
public:
	std::shared_ptr<RawData> raw_data_;
	boost::shared_ptr<Session> session_;
};

#endif // raw_data_with_session_h_
