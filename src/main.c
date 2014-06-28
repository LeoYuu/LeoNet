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
  int len;
  char buf[32];
  net_session* session;
  struct about_crypt* ac;
  const char* connection_ip;

  connection_ip = inet_ntoa(sin->sin_addr);
  printf("accept connection[socket:%d][ip:%s][port:%d].\n", fd, connection_ip, sin->sin_port);

  session = session_manager::instance()->claim_one_session();
  if(session) {
    session->set_socket(fd);
    session_manager::instance()->insert_session(fd, session);

    ac = session->get_crypt();
    len = send_key(ac, buf, sizeof(buf));

  } else {
    printf("%s : can't find one free session\n", __FUNCTION__);
    return;
  }
}

void
on_read(evutil_socket_t fd, void* args) {
  int len;
  ring_buffer* rb;
  net_session* session;
  net_message* message;
  struct about_crypt* ac;
  unsigned short message_len;

  session = session_manager::instance()->get_one_session(fd);
  if(session) {
    rb = session->get_rb();
    ac = session->get_crypt();
    len = net_socket_recv(fd, rb->write_p(), rb->available());
    if(len > 0) {
      rb->seek_write(len);

      do
      {
        len = session->preread_message_size((char*)&message_len, sizeof(message_len));
        if(len < sizeof(message_len)) {
          break;
        }

        message_len ^= (ac->session_key & 0x0000ffff);
        if(message_len > rb->used()) {
          break;
        }

        message = new net_message();
        if(0 == message) {
          printf("on_read: malloc message failed!\n");
          break;
        }

        transform_buffer_to_message(ac, message, rb->read_p(), message_len);
        rb->seek_read(message_len);
        session->push_to_readqueue(message);

      }while(true);
    
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
  ring_buffer* wb;
  net_session* session;
  net_message* message;
  struct about_crypt* ac;

  session = session_manager::instance()->get_one_session(fd);
  if(session) {
    wb = session->get_wb();
    ac = session->get_crypt();
    do 
    {
      ret = session->fetch_from_writequeue(message);
      if(ret) {
        len = transform_message_to_buffer(ac, message, wb->write_p(), message->get_real_size());
        if(len > 0) {
          wb->seek_write(len);
        }
      }
    }while(ret);
    
    len = net_socket_send(fd, wb->read_p(), wb->used());
    if(len > 0) {
      if(wb->empty()) {
        session->del_write_event();
      }
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
