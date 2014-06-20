#include <stdio.h>
#include <WinSock2.h>

#include "util.h"
#include "leo_net_service.h"
#include "leo_net_session.h"
#include "leo_tcp_game_protocol.h"

#define PORT 6000
#define BACKLOG 100

void
on_accept(evutil_socket_t fd, struct sockaddr_in* sin, void* args) {
  const char* connection_ip;
  struct service_init* si;
  net_session* session;

  connection_ip = inet_ntoa(sin->sin_addr);
  printf("accept connection[socket:%d][ip:%s][port:%d].\n", fd, connection_ip, sin->sin_port);

  session = session_manager::instance()->claim_one_session();
  if(session) {
    session->set_socket(fd);
    session_manager::instance()->insert_session(fd, session);
  } else {
    printf("%s : can't find one free session\n", __FUNCTION__);
    return;
  }
}

void
on_read(evutil_socket_t fd, void* args) {
  int len;
  int message_len;
  net_session* session;
  char buf[MAX_BUFFER_LEN];

  session = session_manager::instance()->get_one_session(fd);
  if(session) {
    len = net_socket_recv(fd, buf, sizeof(buf));
    if(len > 0) {
      session->push_to_readbuffer(buf, len);

      do
      {
        message_len = session->peek_message_size();
        /* 根据协议填充消息包 */

      }while(session->readbuffer_used_size() >= message_len);
    
    } else {

    }
  } else {
    printf("%s : can't find the session[fd:%d]!", __FUNCTION__, fd);
    return;
  }
}

void
on_write(evutil_socket_t fd, void* args) {
  int len;
  bool ret;
  char buf[MAX_BUFFER_LEN];
  net_session* session;
  net_message* message;

  session = session_manager::instance()->get_one_session(fd);
  if(session) {
    do 
    {
      ret = session->fetch_from_writequeue(message);
      if(ret) {
        /* 根据消息包填充buffer */
        len = transform_message_to_buffer(message, buf, sizeof(buf));
        if(len > 0) {
          session->push_to_writebuffer(buf, len);
        }
      }
    }while(ret);
    
    len = session->fetch_from_writebuffer(buf, sizeof(buf));
    len = net_socket_send(fd, buf, len);
    if(len > 0) {
      
    } else {

    }
  } else {
    printf("%s : can't find the session[fd:%d]!", __FUNCTION__, fd);
    return;
  }
}

int
main(int argc, char* argv[]) {
  struct event* e;
  struct service_init si;
  evutil_socket_t listen_fd;

#ifdef WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
    printf("Error at WSAStartup()\n");
#endif

  listen_fd = net_socket_open();
  if(-1 == listen_fd) {
    return -1;
  }

  if(net_socket_bind(listen_fd, PORT) < 0) {
    return -1;
  }

  if(net_socket_listen(listen_fd, BACKLOG) < 0) {
    return -1;
  }

  session_manager::create_singleton();
  session_manager::instance()->init_sessions();

  si.ui.__accept_cb = on_accept;
  si.ui.__read_cb = on_read;
  si.ui.__write_cb = on_write;

  si.eb = net_service_create();
  if(NULL == si.eb) {
    return -1;
  }

  e = net_event_create(si.eb, listen_fd, EV_READ | EV_PERSIST, net_event_accept, &si);
  if(NULL == e) {
    return -1;
  }

  event_add(e, NULL);
  event_base_dispatch(si.eb);

#ifdef WIN32
  WSACleanup();
#endif
  return 0;
}
