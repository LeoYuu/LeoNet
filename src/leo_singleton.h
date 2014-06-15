#ifndef __LEO_SINGLETON_H__
#define __LEO_SINGLETON_H__

#include <assert.h>

template <typename t>
class leo_singleton
{
protected:
  leo_singleton() {}
  ~leo_singleton() {}
  leo_singleton& operator=(const leo_singleton&) {}

public:
  inline static void create_singleton()
  {
    assert(__instance);
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
    assert(__instance); 
    return __instance; 
  }

protected:
  static t* __instance;
};

template <typename t>
t* leo_singleton<t>::__instance = NULL;

#endif /* __LEO_SINGLETON_H__ */