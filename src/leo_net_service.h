#ifndef __LEO_NET_SERVICE_H__
#define __LEO_NET_SERVICE_H__

#include "event2/event.h"

#if defined(WIN32) && defined(_BUILD_DLL)
# define LEO_EXPORT __declspec(dllexport)
#else
#  define LEO_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void(* accept_cb)(evutil_socket_t, struct sockaddr_in*, void*);
typedef void(* read_cb)(evutil_socket_t, void*);
typedef void(* write_cb)(evutil_socket_t, void*);
typedef void(* event_cb)(evutil_socket_t, short, void*);

struct net_rw_event {
  struct event* ev_read;
  struct event* ev_write;
};

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
LEO_EXPORT evutil_socket_t net_socket_open();
LEO_EXPORT void net_socket_close(evutil_socket_t fd);
LEO_EXPORT int net_socket_init(evutil_socket_t fd);
 
LEO_EXPORT int net_socket_bind(evutil_socket_t fd, short port);
LEO_EXPORT int net_socket_listen(evutil_socket_t fd, int backlog);
 
/* read & write on socket */
LEO_EXPORT int net_socket_recv(evutil_socket_t fd, char* buf, int len);
LEO_EXPORT int net_socket_send(evutil_socket_t fd, char* buf, int len);
 
/* socket settings */
LEO_EXPORT int net_socket_tcpnodely(evutil_socket_t fd);
LEO_EXPORT int net_socket_reuseaddr(evutil_socket_t fd);
LEO_EXPORT int net_socket_nonblocking(evutil_socket_t fd);

/* create & destory event_base */
LEO_EXPORT struct event_base* net_service_create();
LEO_EXPORT void net_service_release(struct event_base* eb);

/* about events */
LEO_EXPORT struct event* net_event_create(struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg);
LEO_EXPORT void net_event_release(struct event* e);
LEO_EXPORT int net_event_add(struct event* e, const struct timeval* tv);
LEO_EXPORT int net_event_del(struct event* e);
LEO_EXPORT int net_event_reset(struct event* e, struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg);
 
/* callback function for accept/read/write event */
LEO_EXPORT void net_event_accept(evutil_socket_t fd, short events, void* args);
LEO_EXPORT void net_event_read(evutil_socket_t fd, short events, void* args);
LEO_EXPORT void net_event_write(evutil_socket_t fd, short events, void* args);

/* get rw events */
LEO_EXPORT const struct net_rw_event* net_service_get_evnets(evutil_socket_t fd);

#ifdef __cplusplus
  }
#endif

#endif /* __LEO_NET_SERVICE_H__ */
