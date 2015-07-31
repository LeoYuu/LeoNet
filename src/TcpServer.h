#include <thread>
#include "leo_net_service.h"

class TcpServer
{
public:
  TcpServer() {}
  ~TcpServer() {}

public:
  TcpServer(TcpServer&&) = delete;
  TcpServer(const TcpServer&) = delete;

public:
  void Start();
  void Process(void* args);

public:
  void Init(service_init si)
  {
    __si.ssi = si.ssi;
    __si.sui = si.sui;
    __si.eb = nullptr;
  }

public:
  void OnRead(evutil_socket_t fd, void* args);
  void OnWrite(evutil_socket_t fd, void* args);
  void OnError(evutil_socket_t fd, short error, void* args);
  void OnAccept(evutil_socket_t fd, struct sockaddr_in* sin, void* args);

private:
  service_init __si;
  std::thread __thread;
};