#ifndef __LEO_NET_SERVICE_H__
#define __LEO_NET_SERVICE_H__

#include "event2/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void(* accept_cb)(evutil_socket_t, struct sockaddr_in*, void*);
typedef void(* read_cb)(evutil_socket_t, void*);
typedef void(* write_cb)(evutil_socket_t, void*);
typedef void(* event_cb)(evutil_socket_t, short, void*);

struct user_init {
  read_cb __read_cb;
  write_cb __write_cb;
  accept_cb __accept_cb;
};

struct service_init{
  struct user_init ui;
  struct event_base* eb;
};

/* open & close socket */
evutil_socket_t net_socket_open();
void net_socket_close(evutil_socket_t fd);
int net_socket_init(evutil_socket_t fd);

int net_socket_bind(evutil_socket_t fd, short port);
int net_socket_listen(evutil_socket_t fd, int backlog);

/* read & write on socket */
int net_socket_recv(evutil_socket_t fd, char* buf, int len);
int net_socket_send(evutil_socket_t fd, char* buf, int len);

/* socket settings */
int net_socket_tcpnodely(evutil_socket_t fd);
int net_socket_reuseaddr(evutil_socket_t fd);
int net_socket_nonblocking(evutil_socket_t fd);

/* create & destory event_base */
struct event_base* net_service_create();
void net_service_release(struct event_base* eb);

/* callback function for accept/read/write event */
void net_service_accept(evutil_socket_t fd, short events, void* args);
void net_service_read(evutil_socket_t fd, short events, void* args);
void net_service_write(evutil_socket_t fd, short events, void* args);

struct event* net_event_create(struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg);
void net_event_release(struct event* e);
int net_event_reset(struct event* e, struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg);

#ifdef __cplusplus
  }
#endif

#endif /* __LEO_NET_SERVICE_H__ */
