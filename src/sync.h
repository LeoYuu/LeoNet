#ifndef __SYNC_H__
#define __SYNC_H__

#include <pthread.h>

class mutex
{
public:
  mutex()
  {
    pthread_mutex_init(&__mutex, 0);
  }

  mutex(int s, int t)
  {
    pthread_mutexattr_t _attr;

    pthread_mutexattr_init(&_attr);
    pthread_mutexattr_setpshared(&_attr, s);
    pthread_mutexattr_settype(&_attr, t);
    
    pthread_mutex_init(&__mutex, &_attr);
    pthread_mutexattr_destroy(&_attr);
  }

public:
  ~mutex()
  {
    pthread_mutex_destroy(&__mutex);
  }

public:
  int trylock()
  {
    return pthread_mutex_trylock(&__mutex);
  }

  int lock()
  {
    return pthread_mutex_lock(&__mutex);
  }

  int unlock()
  {
    return pthread_mutex_unlock(&__mutex);
  }

private:
  pthread_mutex_t __mutex;
};

class auto_mutex
{
public:
  auto_mutex(mutex* _mutex)
  {
    __mutex = _mutex;
    if(__mutex)
    {
      __mutex->lock();
    }
  }

  ~auto_mutex()
  {
    if(__mutex)
    {
      __mutex->unlock();
    }
  }

private:
  mutex* __mutex;
};

#endif /* __SYNC_H__ */