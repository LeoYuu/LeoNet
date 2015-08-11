#include <memory>
#include "leo_net_address.h"
#include "leo_net_message.h"
#include "leo_lock_free_queue.h"
#include "leo_tcp_game_protocol.h"

#define MAX_QUEUE_LEN 128

typedef std::shared_ptr<net_message> MessagePtr;
typedef static_lock_free_queue<MessagePtr, MAX_QUEUE_LEN> LFMQ;

class TcpConnection
{
public:

public:
  void SendMessage(MessagePtr pMsg)
  {
    if (pMsg)
    {
      pushToWriteQueue(pMsg);
    }
    else
    {
      assert(false);
    }
  }

public:
  inline bool pushToReadQueue(MessagePtr pMsg)
  {
    return readQueue_.push_back(pMsg);
  }

  inline bool fetchFromReadQueue(MessagePtr& pMsg)
  {
    return readQueue_.pop_front(pMsg);
  }

  inline bool pushToWriteQueue(MessagePtr pMsg)
  {
    return writeQueue_.push_back(pMsg);
  }

  inline bool fetchFromWriteQueue(MessagePtr& pMsg)
  {
    return writeQueue_.pop_front(pMsg);
  }

private:
  int socket_;
  LFMQ readQueue_;      /* using by net & logic thread(thread safe). */
  LFMQ writeQueue_;     /* using by net & logic thread(thread safe). */
  about_crypt crypt_;
  NetAddress localAddr_;
  NetAddress remoteAddr_;
};