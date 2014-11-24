#include "threadpool.h"

thread* threadpool::create_thread(thread_attr* _ta, task* _task)
{
  thread* __thread = new thread();
  if(__thread)
  {
    __thread->init(_ta, _task);
  }

  return __thread;
}

void threadpool::destroy_thread(thread* _thread)
{
  if(_thread)
  {
    delete _thread;
    _thread = 0;
  }
}
