#ifndef __LEO_NET_SESSION_H__
#define __LEO_NET_SESSION_H__

#include <map>
#include <vector>

#include "message_define.h"
#include "leo_singleton.h"
#include "leo_net_message.h"
#include "leo_lock_free_queue.h"
#include "leo_tcp_game_protocol.h"

#define MAX_SESSION 10
#define MAX_QUEUE_LEN 128
#define MAX_BUFFER_LEN 4096

class net_session
{
public:
  net_session();
  virtual ~net_session();

public:
  void send_message(net_message* nm);

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

class client_session : public net_session
{
public:
  client_session();
  virtual ~client_session();

public:

private:

};

class server_session : public net_session
{
public:
  server_session();
  virtual ~server_session();
  typedef void (server_session::*net_callback)(const net_message* nm);

public:
  void foreach_process_message();
  void register_all_message_handler();
  void process_message(net_message* nm);

  /* 消息处理函数 */
private:
  void recv_x2x_message_heart(const net_message* nm);

private:
  inline void register_message_handler(unsigned short _id, net_callback _handler)
  {
    if(!__message_handler[_id])
    {
      __message_handler[_id] = _handler;
    }
    else
    {
      assert(false);
    }
  }

private:
  static net_callback __message_handler[max_message];
};

typedef std::vector<server_session*> VCTSESSION;
typedef std::map<int, server_session*> MAPSESSION;

class session_manager
{
public:
  session_manager();
  ~session_manager();

public:
  bool init_sessions();
  int generate_session_id();

  server_session* claim_one_session();
  void reclaim_one_session(server_session* session);

  server_session* get_one_session(int socket);
  bool insert_session(int socket, server_session* session);
  bool remove_session(int socket);

public:
  void dispatch_message();

private:
  unsigned int __session_id;
  VCTSESSION __vct_sessions;
  MAPSESSION __map_sessions;
  server_session* __malloc_session;
};

#endif /* __LEO_NET_SESSION_H__ */
