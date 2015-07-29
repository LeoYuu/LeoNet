#ifndef __LEO_NET_SERVICE_H__
#define __LEO_NET_SERVICE_H__

#include <functional>

#include "event2/util.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"

#include "leo_base_socket.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define C_CALL TRUE
#ifdef C_CALL
  typedef void(* conn_cb)(evutil_socket_t, short);
  typedef void(* read_cb)(evutil_socket_t, void*);
  typedef void(* write_cb)(evutil_socket_t, void*);
  typedef void(* error_cb)(evutil_socket_t, short, void*);
  typedef void(* accept_cb)(evutil_socket_t, struct sockaddr_in*, void*);
#else
  typedef std::function<void(evutil_socket_t, short)> conn_cb;
  typedef std::function<void(evutil_socket_t, void*)> read_cb;
  typedef std::function<void(evutil_socket_t, void*)> write_cb;
  typedef std::function<void(evutil_socket_t, short, void*)> error_cb;
  typedef std::function<void(evutil_socket_t, struct sockaddr_in*, void*)> accept_cb;
#endif /* C_CALL */

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
  error_cb _error_cb;
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

typedef void(* event_cb)(evutil_socket_t, short, void*);

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
