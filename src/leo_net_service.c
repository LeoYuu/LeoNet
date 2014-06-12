#include "leo_net_service.h"

struct net_rw_event {
  struct event* ev_read;
  struct event* ev_write;
};

static struct net_rw_event net_events[65535] = { 0 };

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
net_socket_recv(evutil_socket_t fd, char* buf, int len) {
  int n = 0;
#ifdef _WIN32
  n = recv(fd, buf, len, 0);
#else
  n = read(fd, buf, len);
#endif
  return n;
}

int
net_socket_send(evutil_socket_t fd, char* buf, int len) {
  int n = 0;
#ifdef _WIN32
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
  int slen;
  evutil_socket_t fd;
  struct net_rw_event* ne;
  struct service_init* si;
  struct sockaddr_storage ss;

  slen = sizeof(ss);
  si = (struct service_init*)args;

  fd = accept(listen_fd, (struct sockaddr*)&ss, &slen);
  if(fd < 0) {
    return;
  }

  net_socket_nonblocking(fd);

  ne = (struct net_rw_event*)malloc(sizeof(struct net_rw_event));
  if(ne) {
    ne->ev_read = net_event_create(si->eb, fd, EV_READ | EV_PERSIST, net_service_read, args);
    ne->ev_write = net_event_create(si->eb, fd, EV_WRITE, net_service_write, args);

    if(!ne->ev_read || !ne->ev_write) {
      ne->ev_read ? free(ne->ev_read) : 0;
      ne->ev_write ? free(ne->ev_write) : 0;
      free(ne);
      ne = 0;
    }
  }

  event_add(ne->ev_read, NULL);
  event_add(ne->ev_write, NULL);

  si->ui.__accept_cb(fd, (struct sockaddr_in*)&ss, NULL);
}

void
net_service_read(evutil_socket_t fd, short events, void* args) {
  struct service_init* si;

  si = (struct service_init*)args;

  si->ui.__read_cb(fd, NULL);
}

void
net_service_write(evutil_socket_t fd, short events, void* args) {
  int result;
  struct service_init* si;

  si = (struct service_init*)args;
  
  /*result = si->ui.__write_cb(fd, NULL);

  if(0 == result) {
    
  } else if(result < 0) {

  }*/
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

