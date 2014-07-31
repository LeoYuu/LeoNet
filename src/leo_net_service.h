#ifndef __LEO_NET_SERVICE_H__
#define __LEO_NET_SERVICE_H__

#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif

#include "event2/util.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"

#if defined(WIN32) && defined(_BUILD_DLL)
# define LEO_EXPORT __declspec(dllexport)
#else
# define LEO_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void(* conn_cb)(evutil_socket_t, short);
typedef void(* read_cb)(evutil_socket_t, void*);
typedef void(* write_cb)(evutil_socket_t, void*);
typedef void(* event_cb)(evutil_socket_t, short, void*);
typedef void(* accept_cb)(evutil_socket_t, struct sockaddr_in*, void*);

struct client_user_init{
  read_cb _read_cb;
  write_cb _write_cb;
  conn_cb _connect_cb;
};

struct client_system_init{
  struct sockaddr_in sin;
};

 /* 
 * 作为client时的初始化结构 
 */
struct client_init{
  struct event_base* eb;
  struct client_user_init cui;
  struct client_system_init csi;
};

struct server_user_init{
  read_cb _read_cb;
  write_cb _write_cb;
  accept_cb _accept_cb;
  event_cb _error_cb;
};

struct server_system_init{
  unsigned short _port;
  unsigned int _backlog;
};

 /* 
 * 作为server时的初始化结构 
 */
struct service_init{
  struct event_base* eb;
  struct server_user_init sui;
  struct server_system_init ssi;
};

/* open & close socket */
LEO_EXPORT evutil_socket_t net_socket_open();
LEO_EXPORT void net_socket_close(evutil_socket_t fd);
LEO_EXPORT int net_socket_init(evutil_socket_t fd);

LEO_EXPORT int net_socket_bind(evutil_socket_t fd, short port);
LEO_EXPORT int net_socket_listen(evutil_socket_t fd, int backlog);
 
/* read & write on socket */
LEO_EXPORT int net_socket_recv(evutil_socket_t fd, void* buf, ev_ssize_t len);
LEO_EXPORT int net_socket_send(evutil_socket_t fd, const void* buf, ev_ssize_t len);
 
/* socket settings */
LEO_EXPORT int net_socket_tcpnodely(evutil_socket_t fd);
LEO_EXPORT int net_socket_reuseaddr(evutil_socket_t fd);
LEO_EXPORT int net_socket_nonblocking(evutil_socket_t fd);

/* about events */
LEO_EXPORT struct event* net_event_create(struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg);
LEO_EXPORT void net_event_release(struct event* e);
LEO_EXPORT int net_event_add(struct event* e, const struct timeval* tv);
LEO_EXPORT int net_event_del(struct event* e);
LEO_EXPORT int net_event_reset(struct event* e, struct event_base* eb, evutil_socket_t fd, short events, event_cb cb, void* arg);

/* create & destory event base */
LEO_EXPORT struct event_base* net_core_create();
LEO_EXPORT void net_core_release(struct event_base* eb);
LEO_EXPORT int net_core_loop(struct event_base* eb);

/* create & destory service */
LEO_EXPORT int net_service_create(struct service_init* si);
LEO_EXPORT int net_service_release(struct service_init* si);
 
/* callback function for server accept/read/write event */
LEO_EXPORT void server_event_accept(evutil_socket_t fd, short events, void* args);
LEO_EXPORT void server_event_read(struct bufferevent* bev, void* args);
LEO_EXPORT void server_event_write(struct bufferevent* bev, void* args);
LEO_EXPORT void server_event_error(struct bufferevent* bev, short what, void* args);

/* create & destory client */
LEO_EXPORT int net_client_create(struct client_init* ci);
LEO_EXPORT int net_client_release(struct client_init* ci);

/* callback function for client connect/read/write event */
LEO_EXPORT void client_event_connect(struct bufferevent* bev, short events, void* args);
LEO_EXPORT void client_event_read(struct bufferevent* bev, void* args);
LEO_EXPORT void client_event_write(struct bufferevent* bev, void* args);

#ifdef __cplusplus
  }
#endif

#endif /* __LEO_NET_SERVICE_H__ */
