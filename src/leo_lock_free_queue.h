#ifndef __LEO_LOCK_FREE_QUEUE_H__
#define __LEO_LOCK_FREE_QUEUE_H__

/* 单生产者、单消费者模型 */
template <typename t, int n>
class static_lock_free_queue
{
public:
  void init_queue()
  {
    __push_index = 0;
    __pop_index = 0;
    memset(__queue, 0, sizeof(__queue));
  }

  bool push(t _t)
  {
    if(__push_index - __pop_index < n)
    {
      __queue[__push_index & (n - 1)] = _t;
      __push_index++;
      return true;
    }
    return false;
  }

  bool pop(t& _t)
  {
    if(__push_index != __pop_index)
    {
      _t = __queue[__pop_index & (n - 1)];
      __pop_index++;
      return true;
    }
    return false;
  }

private:
  t __queue[n];
  unsigned int __push_index;
  unsigned int __pop_index;
};

#endif /* __LEO_LOCK_FREE_QUEUE_H__ */