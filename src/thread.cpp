#include "thread.h"

thread::thread()
{

}

thread::~thread()
{

}

int thread::init(thread_attr* ta)
{
  int status;
  pthread_attr_t _thread_attr;

  status = pthread_attr_init(&_thread_attr);
  if(status != 0)
  {
    return -1;
  }

  if(ta->__is_detach)
  {
    status = pthread_attr_setdetachstate(&_thread_attr, PTHREAD_CREATE_DETACHED);
    if(status != 0)
    {
      pthread_attr_destroy(&_thread_attr);
      return -2;
    }
  }

  if(ta->__is_setscope)
  {
    status = pthread_attr_setscope(&_thread_attr, PTHREAD_SCOPE_SYSTEM);
    if(status != 0)
    {
      pthread_attr_destroy(&_thread_attr);
      return -3;
    }
  }

  status = pthread_create(&__tid, &_thread_attr, ta->__thread_cd, ta->__ud);
  pthread_attr_destroy(&_thread_attr);
  
  return status;
}

void* work_thread::to_do(void* ud)
{
  __task->doing();

  return 0;
}

