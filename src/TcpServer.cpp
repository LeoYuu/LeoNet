#include <functional>
#include "TcpServer.h"
#include "server_session.h"
#include "leo_tcp_game_protocol.h"

void
TcpServer::Start()
{
  __thread = std::thread(std::bind(&TcpServer::Process, this, std::placeholders::_1), 0);
}

void
TcpServer::Process(void* args)
{
  initial_xor_keys();
  initial_crc32_table();

  __si.eb = net_core_create();
  if (0 == __si.eb) {
    return;
  }

  if (net_service_create(&__si) < 0) {
    return;
  }

  net_core_loop(__si.eb);
}

void
TcpServer::OnAccept(evutil_socket_t fd, struct sockaddr_in* sin, void* args)
{
  int len;
  char buf[32];
  server_session* session;
  struct about_crypt* ac;
  struct bufferevent* bev;
  const char* connection_ip;
  struct evbuffer* write_buffer;

  bev = (struct bufferevent*)args;
  write_buffer = bufferevent_get_output(bev);

  connection_ip = inet_ntoa(sin->sin_addr);
  printf("accept connection[socket:%d][ip:%s][port:%d].\n", fd, connection_ip, sin->sin_port);

  session = __session_manager.claim_one_session();
  if (session) {
    ac = session->get_crypt();
    session->set_socket(fd);
    __session_manager.insert_session(fd, session);

    len = send_key(ac, buf);
    evbuffer_add(write_buffer, buf, len);

  }
  else {
    printf("%s : can't find one free session\n", __FUNCTION__);
    return;
  }
}

void
TcpServer::OnRead(evutil_socket_t fd, void* args)
{
  int len;
  char buf[1024 + 64];
  net_session* session;
  net_message* message;
  struct about_crypt* ac;
  unsigned short message_len;
  struct evbuffer* read_buffer = (struct evbuffer*)args;

  session = __session_manager.get_one_session(fd);
  if (session)
  {
    ac = session->get_crypt();
    do
    {
      len = pre_read(read_buffer, (void*)&message_len, sizeof(message_len));
      if (len < (int)sizeof(message_len))
      {
        break;
      }

      message_len ^= (ac->session_key & 0x0000ffff);
      if (message_len + sizeof(message_len) > evbuffer_get_length(read_buffer))
      {
        break;
      }

      evbuffer_remove(read_buffer, buf, message_len + sizeof(message_len));

      message = new net_message();
      if (NULL == message)
      {
        printf("on_read: malloc message failed!");
        break;
      }

      transform_buffer_to_message(ac, message, buf, message_len);
      session->push_to_readqueue(message);

    } while (true);
  }
  else
  {
    printf("%s : can't find the session[fd:%d]!", __FUNCTION__, fd);
  }
  return;
}

void
TcpServer::OnWrite(evutil_socket_t fd, void* args)
{
  int len;
  bool ret;
  char buf[1024 + 64];
  net_session* session;
  net_message* message;
  struct about_crypt* ac;
  struct evbuffer* write_buffer = (struct evbuffer*)args;

  session = __session_manager.get_one_session(fd);
  if (session)
  {
    ac = session->get_crypt();
    do
    {
      message = NULL;
      ret = session->fetch_from_writequeue(message);
      if (ret)
      {
        len = transform_message_to_buffer(ac, message, buf, sizeof(buf));
        evbuffer_add(write_buffer, buf, len);
      }
    } while (ret);
  }
  else
  {
    printf("%s : can't find the session[fd:%d]!", __FUNCTION__, fd);
    return;
  }
}

void
TcpServer::OnError(evutil_socket_t fd, short error, void* args)
{
  if (error & BEV_EVENT_EOF)
  {
    /* connection has been closed, do any clean up here */
    printf("close connection!\n");
  }
  else if (error & BEV_EVENT_ERROR)
  {
    /* check errno to see what error occurred */
  }
  else if (error & BEV_EVENT_TIMEOUT)
  {
    /* must be a timeout event handle, handle it */
  }
}