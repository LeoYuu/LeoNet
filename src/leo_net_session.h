#ifndef __LEO_NET_SESSION_H__
#define __LEO_NET_SESSION_H__

#include <map>
#include <vector>

#include "ring_buffer.h"
#include "leo_singleton.h"

#define MAX_SESSION 10
#define MAX_BUFFER_LEN 4096

class net_session
{
public:
  net_session();
  virtual ~net_session();

public:
  void init();

public:
  int push_to_readbuffer(char* buf, int len);
  int fetch_from_readbuffer(char* buf, int len);
  int push_to_writebuffer(char* buf, int len);
  int fetch_from_writebuffer(char* buf, int len);

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

typedef std::vector<net_session*> VCTSESSION;
typedef std::map<int, net_session*> MAPSESSION;

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

  net_session* get_one_session(int socket);
  BOOL insert_session(int socket, net_session* session);
  BOOL remove_session(int socket);

private:
  int __session_id;
  VCTSESSION __vct_sessions;
  MAPSESSION __map_sessions;
  net_session* __malloc_session;
};

#endif /* __LEO_NET_SESSION_H__ */
