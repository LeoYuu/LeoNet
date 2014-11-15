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

int
net_service_create(struct service_init* si) {
  struct event* e;
  unsigned short port;
  unsigned int backlog;
  evutil_socket_t listen_fd;

  port = si->ssi._port;
  backlog = si->ssi._backlog;

  listen_fd = net_socket_open();
  if(-1 == listen_fd) {
    return -1;
  }

  /*si->eb = net_core_create();
  if(NULL == si->eb) {
    return -1;
  }*/

  e = net_event_create(si->eb, listen_fd, EV_READ | EV_PERSIST, server_event_accept, si);
  if(NULL == e) {
    return -1;
  }

  if(net_socket_bind(listen_fd, port) < 0) {
    return -1;
  }

  if(net_socket_listen(listen_fd, backlog) < 0) {
    return -1;
  }

  if(net_socket_nonblocking(listen_fd) < 0) {
    return -1;
  }

  if(net_socket_reuseaddr(listen_fd) < 0) {
    return -1;
  }

  event_add(e, NULL);

  return 0;
}

int
net_service_release(struct service_init* si) {
  return 0;
}

struct event_base*
net_core_create() {
  return event_base_new();
}

void
net_core_release(struct event_base* eb) {
  event_base_free(eb);
}

int
net_core_loop(struct event_base* eb) {
  return event_base_dispatch(eb);
}

void
server_event_accept(evutil_socket_t listen_fd, short events, void* args) {
  ev_socklen_t slen;
  evutil_socket_t fd;
  struct bufferevent* bev;
  struct service_init* si;
  struct sockaddr_storage ss;

  slen = sizeof(ss);
  si = (struct service_init*)args;

  fd = accept(listen_fd, (struct sockaddr*)&ss, &slen);
  if(fd < 0) {
    return;
  }

  net_socket_nonblocking(fd);

  bev = bufferevent_socket_new(si->eb, fd, BEV_OPT_CLOSE_ON_FREE);
  if(bev) {
    bufferevent_setcb(bev, server_event_read, server_event_write, server_event_error, args);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    si->sui._accept_cb(fd, (struct sockaddr_in*)&ss, (void*)bev);
  } else {

  }
}

void
server_event_read(struct bufferevent* bev, void* args) {
  evutil_socket_t fd;
  struct service_init* si;
  struct evbuffer* __read_buffer;

  si = (struct service_init*)args;

  fd = bufferevent_getfd(bev);
  __read_buffer = bufferevent_get_input(bev);

  si->sui._read_cb(fd, (void*)__read_buffer);
}

void
server_event_write(struct bufferevent* bev, void* args) {
  evutil_socket_t fd;
  struct service_init* si;
  struct evbuffer* __write_buffer;

  si = (struct service_init*)args;

  fd = bufferevent_getfd(bev);
  __write_buffer = bufferevent_get_output(bev);

  si->sui._write_cb(fd, (void*)__write_buffer);
}

void
server_event_error(struct bufferevent* bev, short error, void* args) {
  evutil_socket_t fd;
  struct service_init* si;

  fd = bufferevent_getfd(bev);
  si = (struct service_init*)args;

  si->sui._error_cb(fd, error, args);

  net_socket_close(fd);
  bufferevent_free(bev);
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
net_event_add(struct event* e, const struct timeval* tv) {
  return event_add(e, tv);
}

int
net_event_del(struct event* e) {
  return event_del(e);
}

int
net_event_reset(struct event* e, struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg) {
  return event_assign(e, eb, fd, events, cb, arg);
}

int
net_client_create(struct client_init* ci) {
  int ret;
  evutil_socket_t fd;
  struct bufferevent* bev;

  fd = net_socket_open();
  if(fd < 0) {
    return -1;
  }

  bev = bufferevent_socket_new(ci->eb, fd, BEV_OPT_CLOSE_ON_FREE);
  if(0 == bev) {
    return -1;
  }

  bufferevent_setcb(bev, client_event_read, client_event_write, client_event_connect, (void*)ci);

  ret = bufferevent_socket_connect(bev, (struct sockaddr*)&(ci->csi.sin), sizeof(struct sockaddr_in));
  if(ret < 0) {
    return -1;
  }

  bufferevent_enable(bev , EV_READ);

  return 0;
}

int
net_client_release(struct client_init* ci) {
  return 0;
}

void
client_event_connect(struct bufferevent* bev, short events, void* args) {
  evutil_socket_t fd;
  struct client_init* ci = (struct client_init*)args;

  fd = bufferevent_getfd(bev);

  ci->cui._connect_cb(fd, events);
}

void
client_event_read(struct bufferevent* bev, void* args) {
  evutil_socket_t fd;
  struct client_init* ci;
  struct evbuffer* __read_buffer;

  ci = (struct client_init*)args;

  fd = bufferevent_getfd(bev);
  __read_buffer = bufferevent_get_input(bev);

  ci->cui._read_cb(fd, (void*)__read_buffer);
}

void
client_event_write(struct bufferevent* bev, void* args) {
  evutil_socket_t fd;
  struct client_init* ci;
  struct evbuffer* __write_buffer;

  ci = (struct client_init*)args;

  fd = bufferevent_getfd(bev);
  __write_buffer = bufferevent_get_output(bev);

  ci->cui._write_cb(fd, (void*)__write_buffer);
}
