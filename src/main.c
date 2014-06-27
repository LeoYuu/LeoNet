#include <stdio.h>
#include <WinSock2.h>

#include "util.h"
#include "leo_net_service.h"
#include "leo_net_session.h"
#include "leo_tcp_game_protocol.h"

#define PORT 6000
#define BACKLOG 100

static ev_ssize_t pre_read(struct evbuffer* rb, void* buf, size_t len) {
  return evbuffer_copyout(rb, buf, len);
}

void
on_accept(evutil_socket_t fd, struct sockaddr_in* sin, void* args) {
  int len;
  char buf[32];
  net_session* session;
  struct about_crypt* ac;
  struct bufferevent* bev;
  const char* connection_ip;
  struct evbuffer* write_buffer;

  bev = (struct bufferevent*)args;
  write_buffer = bufferevent_get_output(bev);

  connection_ip = inet_ntoa(sin->sin_addr);
  printf("accept connection[socket:%d][ip:%s][port:%d].\n", fd, connection_ip, sin->sin_port);

  session = session_manager::instance()->claim_one_session();
  if(session) {
    ac = session->get_crypt();
    session->set_socket(fd);
    session_manager::instance()->insert_session(fd, session);

    len = send_key(ac, buf);
    evbuffer_add(write_buffer, buf, len);

  } else {
    printf("%s : can't find one free session\n", __FUNCTION__);
    return;
  }
}

void
on_read(evutil_socket_t fd, void* args) {
  int len;
  char buf[1024 + 64];
  net_session* session;
  net_message* message;
  struct about_crypt* ac;
  unsigned short message_len;
  struct evbuffer* read_buffer = (struct evbuffer*)args;
  
  session = session_manager::instance()->get_one_session(fd);
  if(session) {
    ac = session->get_crypt();
    do 
    {
      len = pre_read(read_buffer, (void*)&message_len, sizeof(message_len));
      if(len < (sizeof(message_len))) {
        break;
      }

      message_len ^= (ac->session_key & 0x0000ffff);
      if(message_len + sizeof(message_len) > evbuffer_get_length(read_buffer)) {
        break;
      }

      evbuffer_remove(read_buffer, buf, message_len + sizeof(message_len));

      message = new net_message();
      if(0 == message) {
        printf("on_read: malloc message failed!");
        break;
      }

      transform_buffer_to_message(ac, message, buf, message_len);
      session->push_to_readqueue(message);

    }while(true);
  } else {
    printf("%s : can't find the session[fd:%d]!", __FUNCTION__, fd);
  }
  return;
}

void
on_write(evutil_socket_t fd, void* args) {
  int len;
  bool ret;
  char buf[1024 + 64];
  net_session* session;
  net_message* message;
  struct about_crypt* ac;
  struct evbuffer* write_buffer = (struct evbuffer*)args;

  session = session_manager::instance()->get_one_session(fd);
  if(session) {
    ac = session->get_crypt();
    do 
    {
      message = 0;
      ret = session->fetch_from_writequeue(message);
      if(ret) {
        len = transform_message_to_buffer(ac, message, buf, sizeof(buf));
        evbuffer_add(write_buffer, buf, len);
      }
    }while(ret);
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
  if(WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
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

  initial_xor_keys();
  initial_crc32_table();
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
