#include "leo_marco_util.h"
#include "server_session.h"

server_session::net_callback server_session::__message_handler[max_message] = { 0, };

server_session::server_session()
{

}

server_session::~server_session()
{

}

void server_session::foreach_process_message()
{
  net_message* nm = NULL;

  while(fetch_from_readqueue(nm))
  {
    process_message(nm);
  }
}

void server_session::process_message(net_message* nm)
{
  unsigned short _msg_id = nm->get_id();

  if(_msg_id < max_message && NULL != __message_handler[_msg_id])
  {
    (this->*__message_handler[_msg_id])(nm);

    SAFE_DELETE(nm);
  }
  else
  {
    assert(false);
  }
}

void server_session::register_all_message_handler()
{
#define register_message_handler(id) server_session::register_message_handler(id, &server_session::recv_##id)
  register_message_handler(x2x_message_heart);

#undef register_message_handler
}

session_manager::session_manager()
: __session_id(0)
, __malloc_session(NULL)
{
  __vct_sessions.clear();
  __map_sessions.clear();
}

session_manager::~session_manager()
{
  __session_id = 0;
  __map_sessions.clear();
  __vct_sessions.clear();

  delete[] __malloc_session;
  __malloc_session = NULL;
}

bool session_manager::init_sessions()
{
  __malloc_session = new server_session[MAX_SESSION];
  if(__malloc_session)
  {
    for(int i = 0; i < MAX_SESSION; ++i)
    {
      __vct_sessions.push_back(&__malloc_session[i]);
    }
    return true;
  }

  return false;
}

server_session* session_manager::claim_one_session()
{
  server_session* session = NULL;

  if(__vct_sessions.size())
  {
    session = __vct_sessions.front();
    __vct_sessions.erase(__vct_sessions.begin());
  }

  return session;
}

void session_manager::reclaim_one_session(server_session* session)
{
  assert(session);

  if(session)
  {
    __vct_sessions.push_back(session);
  }

  session = NULL;
}

bool session_manager::insert_session(int socket, server_session* session)
{
  assert(socket > 0 && session);

  MAPSESSION::iterator map_iter = __map_sessions.find(socket);
  if(map_iter == __map_sessions.end())
  {
    __map_sessions.insert(std::make_pair<int, server_session*>(socket, session));

    return true;
  }
  else
  {
    printf("session-manager: insert session error[exist].");
    return false;
  }
}

bool session_manager::remove_session(int socket)
{
  assert(socket > 0);

  MAPSESSION::iterator map_iter = __map_sessions.find(socket);
  if(map_iter == __map_sessions.end())
  {
    printf("session-manager: remove session error[inexist].");
    return false;
  }
  else
  {
    __map_sessions.erase(map_iter);
    return true;
  }
}

server_session* session_manager::get_one_session(int socket)
{
  assert(socket > 0);

  server_session* session = NULL;

  MAPSESSION::iterator map_iter = __map_sessions.find(socket);
  if(map_iter == __map_sessions.end())
  {
    printf("session-manager: get one session error[inexist].\n");
  }
  else
  {
    session = map_iter->second;
  }

  return session;
}

int session_manager::generate_session_id()
{
  return ++__session_id;
}

void session_manager::dispatch_message()
{
  server_session* _message = NULL;
  server_session* _session = NULL;

  for(MAPSESSION::iterator iter = __map_sessions.begin(); iter != __map_sessions.end(); ++iter)
  {
    _session = iter->second;
    if(NULL == _session)
      continue;

    /* 处理每个session的消息 */
    _session->foreach_process_message();
  }
}
