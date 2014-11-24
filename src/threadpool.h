#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "thread.h"

class threadpool
{
public:
  thread* create_thread(thread_attr* _ta, task* _task);
  void destroy_thread(thread* _thread);

private:
  int __pool_size;
};

#endif /* __THREAD_POOL_H__ */
