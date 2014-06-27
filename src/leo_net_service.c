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
net_socket_recv(evutil_socket_t fd, char* buf, int len) {
  int n = 0;
#ifdef WIN32
  n = recv(fd, buf, len, 0);
#else
  n = read(fd, buf, len);
#endif
  return n;
}

int
net_socket_send(evutil_socket_t fd, char* buf, int len) {
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

struct event_base*
net_service_create() {
  return event_base_new();
}

void
net_service_release(struct event_base* eb) {
  event_base_free(eb);
}

void
net_event_accept(evutil_socket_t listen_fd, short events, void* args) {
  int slen;
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
    bufferevent_setcb(bev, net_event_read, net_event_write, net_event_error, args);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    si->ui.__accept_cb(fd, (struct sockaddr_in*)&ss, (void*)bev);
  } else {

  }
}

void
net_event_read(struct bufferevent* bev, void* args) {
  evutil_socket_t fd;
  struct service_init* si;
  struct evbuffer* __read_buffer;

  si = (struct service_init*)args;

  fd = bufferevent_getfd(bev);
  __read_buffer = bufferevent_get_input(bev);

  si->ui.__read_cb(fd, (void*)__read_buffer);
}

void
net_event_write(struct bufferevent* bev, void* args) {
  evutil_socket_t fd;
  struct service_init* si;
  struct evbuffer* __write_buffer;

  si = (struct service_init*)args;

  fd = bufferevent_getfd(bev);
  __write_buffer = bufferevent_get_output(bev);

  si->ui.__write_cb(fd, (void*)__write_buffer);
}

void
net_event_error(struct bufferevent* bev, short error, void* args) {
  evutil_socket_t fd;

  fd = bufferevent_getfd(bev);

  if(error & BEV_EVENT_EOF) {
    /* connection has been closed, do any clean up here */
  } else if(error & BEV_EVENT_ERROR) {
    /* check errno to see what error occurred */
  } else if(error & BEV_EVENT_TIMEOUT) {
    /* must be a timeout event handle, handle it */
  }

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
