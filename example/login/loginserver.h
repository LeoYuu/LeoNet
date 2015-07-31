#ifndef __LEO_LOGINSERVER_H__
#define __LEO_LOGINSERVER_H__

#include <thread>

#include "TcpServer.h"
#include "leo_singleton.h"

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

private:
  TcpServer __tcp_server;
  session_manager __session_manager;
};

#endif /* __LEO_LOGINSERVER_H__ */
