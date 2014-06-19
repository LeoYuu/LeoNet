#ifndef __LEO_NET_MESSAGE_H__
#define __LEO_NET_MESSAGE_H__

#include <memory.h> // for memset

#define BUFFER_SIZE 1024

class net_message
{
public:
  net_message()
    : __id(0)
    , __real_size(0)
  {
    memset(__buffer, 0, sizeof(__buffer));
  }

  inline unsigned short get_id() const
  {
    return __id;
  }

  inline void set_id(unsigned short _id)
  {
    __id = _id;
  }

  inline unsigned short get_real_size() const
  {
    return __real_size;
  }

  inline void set_real_size(unsigned short _size)
  {
    __real_size = _size;
  }

  template <typename t>
  t* pop_ptr()
  {
    t* _t = 0;
    if(__real_size + sizeof(t) < BUFFER_SIZE)
    {
      _t = (t*)&__buffer[__real_size];
      __real_size += sizeof(t);
    }
    return _t;
  }

  template <typename t>
  t& pop_ref()
  {
    if(__real_size + sizeof(t) < BUFFER_SIZE)
    {
      t& _t = *(t*)&__buffer[__real_size];
      __real_size += sizeof(t);
      return _t;
    }
  }
  
private:
  unsigned short __id;
  unsigned short __real_size;
  char __buffer[BUFFER_SIZE];
};

#endif /* __LEO_NET_MESSAGE_H__ */
