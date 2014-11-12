#include "leo_loginserver.h"
#include "leo_net_service.h"

static ev_ssize_t pre_read(struct evbuffer* rb, void* buf, size_t len) {
  return evbuffer_copyout(rb, buf, len);
}

login_server::login_server() {

}

login_server::~login_server() {

}

void login_server::start_network() {
  int ret = pthread_create(&thread_id, 0, &login_server::thread_network, 0);
  if(ret) {

  }
}

void* login_server::thread_network(void* args) {

#ifdef WIN32
  WSADATA wsaData;
  if(WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
    printf("Error at WSAStartup()\n");
#endif

  initial_xor_keys();
  initial_crc32_table();
  session_manager::create_singleton();
  session_manager::instance()->init_sessions();

  __si.ssi._port = PORT;
  __si.ssi._backlog = BACKLOG;
  __si.sui._accept_cb = &login_server::on_loginserver_accept;
  __si.sui._read_cb = &login_server::on_loginserver_read;
  __si.sui._write_cb = &login_server::on_loginserver_write;
  __si.sui._error_cb = &login_server::on_loginserver_error;

  __si.eb = net_core_create();
  if(0 == __si.eb) {
    return 0;
  }

  if(net_service_create(&__si) < 0) {
    return 0;
  }

  net_core_loop(__si.eb);

#ifdef WIN32
  WSACleanup();
#endif

  return 0;
}

void login_server::start_loginserver() {
 
}
