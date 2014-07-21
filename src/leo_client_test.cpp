#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <WinSock2.h>
#endif

#include "leo_net_service.h"
#include "leo_net_session.h"
#include "leo_tcp_game_protocol.h"

#define PORT 6000
#define BACKLOG 100

void
on_read(evutil_socket_t fd, void* args) {
  
}

void
on_write(evutil_socket_t fd, void* args) {
  
}

void
on_connect(evutil_socket_t fd, short events) {
  if(events & BEV_EVENT_CONNECTED) {
    printf("connected!\n");
  } else if(events & BEV_EVENT_ERROR) {
    printf("connect error.\n");
  }
}

int
main(int argc, char* argv[]) {
  struct client_init ci;

#ifdef WIN32
  WSADATA wsaData;
  if(WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
    printf("Error at WSAStartup()\n");
#endif

  initial_xor_keys();
  initial_crc32_table();

  memset(&ci.csi.sin, 0, sizeof(struct sockaddr_in));
  ci.csi.sin.sin_family = AF_INET;
  ci.csi.sin.sin_addr.s_addr = inet_addr("127.0.0.1");
  ci.csi.sin.sin_port = htons(PORT);
  ci.cui._connect_cb = on_connect;
  ci.cui._read_cb = on_read;
  ci.cui._write_cb = on_write;

  ci.eb = net_core_create();
  if(0 == ci.eb) {
    return -1;
  }

  if(net_client_create(&ci) < 0) {
    return -1;
  }

  net_core_loop(ci.eb);

#ifdef WIN32
  WSACleanup();
#endif
  return 0;
}
