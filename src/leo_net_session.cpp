#include "leo_net_service.h"
#include "leo_net_session.h"

net_session::net_session()
: __socket(0)
, __session_id(0)
, __read_buffer(MAX_BUFFER_LEN)
, __write_buffer(MAX_BUFFER_LEN)
{

}

net_session::~net_session()
{
  __read_buffer.clear();
  __write_buffer.clear();
}

void net_session::init()
{

}

// TODO: move to leo_net_base library later.
void net_session::add_write_event()
{
  const struct net_rw_event* rw_events = net_service_get_evnets(get_socket());
  if(rw_events && rw_events->ev_write)
  {
    net_event_add(rw_events->ev_write, 0);
  }
}

void net_session::del_write_event()
{
  const struct net_rw_event* rw_events = net_service_get_evnets(get_socket());
  if(rw_events && rw_events->ev_write)
  {
    net_event_del(rw_events->ev_write);
  }
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
  __malloc_session = new net_session[MAX_SESSION];
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

net_session* session_manager::claim_one_session()
{
  net_session* session = NULL;

  if(__vct_sessions.size())
  {
    session = __vct_sessions.front();
    __vct_sessions.erase(__vct_sessions.begin());
  }

  return session;
}

void session_manager::reclaim_one_session(net_session* session)
{
  assert(session);

  if(session)
  {
    __vct_sessions.push_back(session);
  }

  session = NULL;
}

bool session_manager::insert_session(int socket, net_session* session)
{
  assert(socket > 0 && session);

  MAPSESSION::iterator map_iter = __map_sessions.find(socket);
  if(map_iter == __map_sessions.end())
  {
    __map_sessions.insert(std::make_pair<int, net_session*>(socket, session));

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

net_session* session_manager::get_one_session(int socket)
{
  assert(socket > 0);

  net_session* session = NULL;

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
