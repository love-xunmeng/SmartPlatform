#ifndef common_h_
#define common_h_

//#include "MessageWidthSession.h"
#include "mqueue.h"
//#include "RawData.h"
#include "SmartPlatform.pb.h"
#include <memory>

class MessageWithSession;
class RawDataWithSession;

typedef std::shared_ptr<RawDataWithSession> RawDataPtr;
typedef MQuque<RawDataPtr> RawMessageDataQueue;
typedef std::shared_ptr<RawMessageDataQueue> RawMessageDataQueuePtr;
typedef MQuque<std::shared_ptr<MessageWithSession>> MessageQueue;
typedef std::shared_ptr<MessageQueue> MessageQueuePtr;

#endif common_h_
