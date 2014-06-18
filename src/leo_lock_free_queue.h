#ifndef __LEO_LOCK_FREE_QUEUE_H__
#define __LEO_LOCK_FREE_QUEUE_H__

#include <list>

template <typename t>
class lock_free_queue
{
public:
  lock_free_queue()
  {
    list.push_back(t());
    __head = list.begin();
    __tail = list.end();
  }

  void push(const t& _t)
  {
    list.push_back(_t);
    __tail = list.end();
    list.erase(list.begin(), __head);
  }

  BOOL pop(t& _t)
  {
    std::list<t>::iterator it = __head;
    ++it;
    if(it != __tail)
    {
      __head = it;
      t = *__head;
      return TRUE;
    }
    return FALSE;
  }
private:
  std::list<t> __list;
  std::list<t>::iterator __head;
  std::list<t>::iterator __tail;
};

#endif /* __LEO_LOCK_FREE_QUEUE_H__ */