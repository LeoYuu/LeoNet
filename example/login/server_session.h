#ifndef __SERVER_SESSION_H__
#define __SERVER_SESSION_H__

#include <map>
#include <vector>

#include "leo_singleton.h"
#include "message_define.h"
#include "leo_net_session.h"

#define MAX_SESSION 10

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


#endif /* __SERVER_SESSION_H__ */