#ifndef __LEO_LOGINSERVER_H__
#define __LEO_LOGINSERVER_H__

#include <pthread.h>

#include "leo_singleton.h"
#include "leo_net_service.h"
#include "leo_net_session.h"
#include "leo_tcp_game_protocol.h"

#define PORT 6000
#define BACKLOG 100

class login_server
{
public:
  login_server();
  ~login_server();

public:
  void start_network();
  void start_loginserver();

public:
  void* thread_network(void* args);

public:
  void on_loginserver_read(evutil_socket_t fd, void* args);
  void on_loginserver_write(evutil_socket_t fd, void* args);
  void on_loginserver_error(evutil_socket_t fd, short error, void* args);
  void on_loginserver_accept(evutil_socket_t fd, struct sockaddr_in* sin, void* args);

private:
  service_init __si;
  pthread_t thread_id;
};

extern login_server g_loginserver;

#endif /* __LEO_LOGINSERVER_H__ */
