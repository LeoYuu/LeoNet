#include <stdio.h>
#include <WinSock2.h>

#include "util.h"
#include "leo_net_service.h"
#include "leo_net_session.h"

#define PORT 6000
#define BACKLOG 100

void
on_accept(evutil_socket_t fd, struct sockaddr_in* sin, void* args) {
  const char* connection_ip;
  struct service_init* si;
  net_session* session;

  connection_ip = inet_ntoa(sin->sin_addr);
  printf("accept connection[socket:%d][ip:%s][port:%d].\n", fd, connection_ip, sin->sin_port);

  session = session_manager::instance()->claim_one_session();
  if(session) {
    session_manager::instance()->insert_session(fd, session);
  } else {
    printf("%s : can't find one free session\n", __FUNCTION__);
    return;
  }
}

void
on_read(evutil_socket_t fd, void* args) {
  net_session* session;
  session = session_manager::instance()->get_one_session(fd);
  if(session) {
    
  } else {

  }
}

void
on_write(evutil_socket_t fd, void* args) {

}

int
main(int argc, char* argv[]) {
  struct event* e;
  struct service_init si;
  evutil_socket_t listen_fd;

#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
    printf("Error at WSAStartup()\n");
#endif

  listen_fd = net_socket_open();
  if(-1 == listen_fd) {
    return -1;
  }

  if(net_socket_bind(listen_fd, PORT) < 0) {
    return -1;
  }

  if(net_socket_listen(listen_fd, BACKLOG) < 0) {
    return -1;
  }

  si.ui.__accept_cb = on_accept;
  si.ui.__read_cb = on_read;
  si.ui.__write_cb = on_write;

  si.eb = net_service_create();
  if(NULL == si.eb) {
    return -1;
  }

  e = net_event_create(si.eb, listen_fd, EV_READ | EV_PERSIST, net_service_accept, &si);
  if(NULL == e) {
    return -1;
  }

  event_add(e, NULL);
  event_base_dispatch(si.eb);

#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}
