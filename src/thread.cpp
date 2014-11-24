#include "thread.h"

static void* thread_proc(void* _ud)
{
  thread* __thread = (thread*)_ud;
  if(__thread)
  {
    __thread->to_do();
  }

  return 0;
}

thread::thread()
{
}

thread::~thread()
{

}

int thread::init(thread_attr* _ta, task* _task)
{
  int status;
  pthread_attr_t _thread_attr;

  status = pthread_attr_init(&_thread_attr);
  if(status != 0)
  {
    return -1;
  }

  if(_ta->__is_detach)
  {
    status = pthread_attr_setdetachstate(&_thread_attr, PTHREAD_CREATE_DETACHED);
    if(status != 0)
    {
      pthread_attr_destroy(&_thread_attr);
      return -2;
    }
  }

  if(_ta->__is_setscope)
  {
    status = pthread_attr_setscope(&_thread_attr, PTHREAD_SCOPE_SYSTEM);
    if(status != 0)
    {
      pthread_attr_destroy(&_thread_attr);
      return -3;
    }
  }

  set_task(_task);

  status = pthread_create(&__tid, &_thread_attr, thread_proc, this);
  pthread_attr_destroy(&_thread_attr);
  
  return status;
}

