#ifndef __LEO_NET_SESSION_H__
#define __LEO_NET_SESSION_H__

#include <map>
#include <vector>

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
  void init();

public:
  inline bool push_to_readqueue(net_message* nm)
  {
    return __read_queue.push_back(nm);
  }

  inline bool fetch_from_readqueue(net_message* nm)
  {
    return __read_queue.pop_front(nm);
  }

  inline bool push_to_writequeue(net_message* nm)
  {
    return __write_queue.push_back(nm);
  }

  inline bool fetch_from_writequeue(net_message* nm)
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

  inline void set_session_id(int id)
  {
    __session_id = id;
  }

  inline int get_session_id()
  {
    return __session_id;
  }

  inline struct about_crypt* get_crypt()
  {
    return &__crypt;
  }

private:
  int __socket;
  int __session_id;
  struct about_crypt __crypt;
  static_lock_free_queue<net_message*, MAX_QUEUE_LEN> __read_queue;  /* using by net & logic thread(thread safe). */
  static_lock_free_queue<net_message*, MAX_QUEUE_LEN> __write_queue; /* using by net & logic thread(thread safe). */
};

typedef std::vector<net_session*> VCTSESSION;
typedef std::map<int, net_session*> MAPSESSION;

class session_manager : public leo_singleton<session_manager>
{
public:
  session_manager();
  ~session_manager();

public:
  bool init_sessions();
  int generate_session_id();

  net_session* claim_one_session();
  void reclaim_one_session(net_session* session);

  net_session* get_one_session(int socket);
  bool insert_session(int socket, net_session* session);
  bool remove_session(int socket);

private:
  int __session_id;
  VCTSESSION __vct_sessions;
  MAPSESSION __map_sessions;
  net_session* __malloc_session;
};

#endif /* __LEO_NET_SESSION_H__ */
