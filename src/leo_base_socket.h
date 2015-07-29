#ifndef __LEO_BASE_SOCKET_H__
#define __LEO_BASE_SOCKET_H__

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif

#include "event2/util.h"

#if defined(WIN32) && defined(_BUILD_DLL)
# define LEO_EXPORT __declspec(dllexport)
#else
# define LEO_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* __LEO_BASE_SOCKET_H__ */