#include "leo_net_service.h"

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
net_socket_nonblocking(evutil_socket_t fd) {
  return evutil_make_socket_nonblocking(fd);
}

int
net_socket_reuseaddr(evutil_socket_t fd) {
  int flag = 1;
  if(-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int))) {
    return 0;
  }
  return 1;
}

int
net_socket_tcpnodely(evutil_socket_t fd) {
  int flag = 1;
  if(-1 == setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))) {
    return 0;
  }
  return 1;
}

struct event_base*
net_service_create() {
  return event_base_new();
}

void
net_service_release(struct event_base* eb) {
  event_base_free(eb);
}

void
net_service_accept(evutil_socket_t listen_fd, short events, void* args) {
  struct sockaddr_storage ss;
  int slen = sizeof(ss);
  struct event *ev_read, *ev_write;
  struct service_init* si = (struct service_init*)args;

  evutil_socket_t fd = accept(listen_fd, (struct sockaddr*)&ss, &slen);
  if(fd < 0) {
    return;
  }

  net_socket_nonblocking(fd);

  ev_read = net_event_create(si->eb, fd, EV_READ | EV_PERSIST, net_service_read, args);
  if(!ev_read) {
    return;
  }

  ev_write = net_event_create(si->eb, fd, EV_WRITE | EV_PERSIST, net_service_write, args);
  if(!ev_write) {
    net_event_release(ev_read);
    return;
  }

  event_add(ev_read, NULL);
  event_add(ev_write, NULL);

  si->ui.__accept_cb(fd, (struct sockaddr_in*)&ss, NULL);
}

void
net_service_read(evutil_socket_t fd, short events, void* args) {
  struct service_init* si = (struct service_init*)args;

  si->ui.__read_cb(fd, NULL);
}

void
net_service_write(evutil_socket_t fd, short events, void* args) {
  int result;
  struct service_init* si = (struct service_init*)args;

  result = si->ui.__write_cb(fd, NULL);

  if(0 == result) {
    
  } else if(result < 0) {

  }
}

struct event*
net_event_create(struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg) {
  return event_new(eb, fd, events, cb, arg);
}

void
net_event_release(struct event* e) {
  event_free(e);
}

int
net_event_reset(struct event* e, struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg) {
  return event_assign(e, eb, fd, events, cb, arg);
}

