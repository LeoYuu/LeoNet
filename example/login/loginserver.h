#ifndef __LEO_LOGINSERVER_H__
#define __LEO_LOGINSERVER_H__

#include <thread>

#include "leo_singleton.h"
#include "leo_net_service.h"
#include "leo_tcp_game_protocol.h"
#include "server_session.h"

#define PORT 1200
#define BACKLOG 100

class login_server : public leo_singleton<login_server>
{
public:
  login_server();
  ~login_server();

public:
  /*bool load_ini_config();
  bool load_xml_config();*/

public:
  void start_network();
  void start_loginserver();

public:
  void main_loop();
  void thread_network(void* args);

public:
  void on_loginserver_read(evutil_socket_t fd, void* args);
  void on_loginserver_write(evutil_socket_t fd, void* args);
  void on_loginserver_error(evutil_socket_t fd, short error, void* args);
  void on_loginserver_accept(evutil_socket_t fd, struct sockaddr_in* sin, void* args);

private:
  service_init __si;
  std::thread __thread;
  session_manager __session_manager;
};

#endif /* __LEO_LOGINSERVER_H__ */
