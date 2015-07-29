#include "leo_base_socket.h"


evutil_socket_t
net_socket_open() {
  return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void
net_socket_close(evutil_socket_t fd) {
  evutil_closesocket(fd);
}

int
net_socket_init(evutil_socket_t fd) {
  return 0;
}

int
net_socket_bind(evutil_socket_t fd, short port) {
  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(port);

  return bind(fd, (struct sockaddr*)&sin, sizeof(sin));
}

int
net_socket_listen(evutil_socket_t fd, int backlog) {
  return listen(fd, backlog);
}

int
net_socket_recv(evutil_socket_t fd, void* buf, ev_ssize_t len) {
  int n = 0;
#ifdef WIN32
  n = recv(fd, buf, len, 0);
#else
  n = read(fd, buf, len);
#endif
  return n;
}

int
net_socket_send(evutil_socket_t fd, const void* buf, ev_ssize_t len) {
  int n = 0;
#ifdef WIN32
  n = send(fd, buf, len, 0);
#else
  n = write(fd, buf, len);
#endif
  return n;
}

int
net_socket_nonblocking(evutil_socket_t fd) {
  return evutil_make_socket_nonblocking(fd);
}

int
net_socket_reuseaddr(evutil_socket_t fd) {
  return evutil_make_listen_socket_reuseable(fd);
}

int
net_socket_tcpnodely(evutil_socket_t fd) {
  int flag = 1;
  if(-1 == setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))) {
    return 0;
  }
  return 1;
}
