#ifndef __LEO_SINGLETON_H__
#define __LEO_SINGLETON_H__

#include <assert.h>

#include "leo_utilities.h"

template <typename t>
class LEO_EXPORT leo_singleton
{
protected:
  leo_singleton() {}
  ~leo_singleton() {}
  leo_singleton& operator=(const leo_singleton&) {}

public:
  inline static void create_singleton()
  {
    assert(!__instance);
    if(!__instance)
      __instance = new t;
    assert(__instance);
  }

  inline static void release_singleton()
  {
    assert(__instance);
    delete __instance;
  }

  inline static t* instance()
  { 
    if (!__instance)
    {
      __instance = create_singleton();
    }
    return __instance; 
  }

protected:
  static t* __instance;
};

template <typename t>
t* leo_singleton<t>::__instance = nullptr;

#endif /* __LEO_SINGLETON_H__ */
