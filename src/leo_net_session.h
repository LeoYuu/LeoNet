#ifndef __LEO_NET_SESSION_H__
#define __LEO_NET_SESSION_H__

#include <map>
#include <vector>
#include "ring_buffer.h"

class net_session
{
public:
  net_session();
  virtual ~net_session();

public:
  void init();
  void send_data(char* buf, int len);
  void recv_data(char* buf, int len);

public:
  inline void set_socket(int fd)
  {
    __socket = fd;
  }

  inline int get_socket()
  {
    return __socket;
  }

  inline void set_session_id(int id)
  {
    __session_id = id;
  }

  inline int get_session_id()
  {
    return __session_id;
  }

private:
  int __socket;
  int __session_id;
  ring_buffer __read_buffer;
  ring_buffer __write_buffer;
};

#define MAX_SESSION 2000

typedef vector<net_session*> VCTSESSION;
typedef map<int, net_session*> MAPSESSION;

class session_manager : public leo_singleton<session_manager>
{
public:
  session_manager();
  ~session_manager();

public:
  BOOL init_sessions();
  int generate_session_id();

  net_session* claim_one_session();
  void reclaim_one_session(net_session* session);

  BOOL insert_session(int socket, net_session* session);
  void remove_session(int socket);
  net_session* get_one_session(int socket);

private:
  int __session_id;
  VCTSESSION __vct_sessions;
  MAPSESSION __map_sessions;

  net_session* __malloc_session;
};

#endif /* __LEO_NET_SESSION_H__ */
