#ifndef __LEO_NET_SESSION_H__
#define __LEO_NET_SESSION_H__

#include "leo_net_message.h"
#include "leo_lock_free_queue.h"
#include "leo_tcp_game_protocol.h"

#define MAX_QUEUE_LEN 128

class net_session
{
public:
  net_session()
  : __socket(0)
  , __session_id(0)
  {
    __read_queue.init_queue();
    __write_queue.init_queue();
    memset(&__crypt, 0, sizeof(__crypt));
  }

  virtual ~net_session()
  {

  }

public:
  void send_message(net_message* nm)
  {
    if(nm)
    {
      push_to_writequeue(nm);
    }
    else
    {
      assert(false);
    }
  }

public:
  inline bool push_to_readqueue(net_message* nm)
  {
    return __read_queue.push_back(nm);
  }

  inline bool fetch_from_readqueue(net_message*& nm)
  {
    return __read_queue.pop_front(nm);
  }

  inline bool push_to_writequeue(net_message* nm)
  {
    return __write_queue.push_back(nm);
  }

  inline bool fetch_from_writequeue(net_message*& nm)
  {
    return __write_queue.pop_front(nm);
  }

public:
  inline void set_socket(int fd)
  {
    __socket = fd;
  }

  inline int get_socket()
  {
    return __socket;
  }

  inline void set_session_id(unsigned int id)
  {
    __session_id = id;
  }

  inline unsigned int get_session_id()
  {
    return __session_id;
  }

  inline struct about_crypt* get_crypt()
  {
    return &__crypt;
  }

private:
  int __socket;
  unsigned int __session_id;
  struct about_crypt __crypt;
  static_lock_free_queue<net_message*, MAX_QUEUE_LEN> __read_queue;  /* using by net & logic thread(thread safe). */
  static_lock_free_queue<net_message*, MAX_QUEUE_LEN> __write_queue; /* using by net & logic thread(thread safe). */
};

#endif /* __LEO_NET_SESSION_H__ */
