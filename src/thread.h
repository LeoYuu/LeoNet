#ifndef __THREAD_H__
#define __THREAD_H__

#include <sched.h>
#include <pthread.h>

#include "task.h"

struct thread_attr
{
  bool __is_detach;
  bool __is_setscope;
};

class thread
{
public:
  thread();
  virtual ~thread();

public:
  int init(thread_attr* _ta, task* _task);

public:
  int to_do()
  {
    __task->doing();

    return 0;
  }

public:
  int join(void** _value = 0)
  {
    return pthread_join(__tid, _value);
  }

  int detach()
  {
    return pthread_detach(__tid);
  }

  int exit(void* _value = 0)
  {
    if(is_self())
    {
      pthread_exit(_value);
    }
  }

  void yield()
  {
    sched_yield();
  }

  bool is_self()
  {
    if(pthread_equal(__tid, pthread_self()) != 0)
      return true;
    else
      return false;
  }

  pthread_t get_tid()
  {
    return __tid;
  }

  void set_task(task* _task)
  {
    __task = _task;
  }

protected:
  task* __task;
  pthread_t __tid;
};

#endif /* __THREAD_H__ */