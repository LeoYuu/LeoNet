#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include "leo_utilities.h"

#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#ifdef WIN32
# include <windows.h>
# define mb() MemoryBarrier()
# define rmb() MemoryBarrier()
# define wmb() MemoryBarrier()
#else
# define mb() asm volatile("mfence":::"memory")
# define rmb() asm volatile("lfence":::"memory")
# define wmb() asm volatile("sfence" ::: "memory")
#endif

#define smp_mb() mb()
#define smp_rmb() rmb()
#define smp_wmb() wmb()

// 无锁缓冲队列.
class LEO_EXPORT ring_buffer
{
public:
  explicit ring_buffer(int buf_size);
  ~ring_buffer();

public:
  inline void clear()
  {
    __write_p = 0;
    __read_p = 0;
  }

  inline unsigned int available() const
  {
    return __buffer_size - (__write_p - __read_p);
  }

  inline unsigned int used() const
  {
    return __write_p - __read_p;
  }

  inline bool empty() const
  {
    return used() == 0;
  }

  inline bool full() const
  {
    return __buffer_size == (int)used(); /* 消除警告 */
  }

  inline char* write_p()
  {
    return &__ring_buffer[__write_p & (__buffer_size - 1)];
  }

  inline char* read_p()
  {
    return &__ring_buffer[__read_p & (__buffer_size - 1)];
  }

  /*
  * Read data but don't move the m_read_p.
  */
  unsigned int peek_read(char* buffer, unsigned int len);
  /*
  * Move the m_read_p
  */
  bool seek_read(unsigned int len);

  /*
  * Write data but don't move the m_write_p.
  */
  unsigned int peek_write(char* buffer, unsigned int len);
  /*
  * Move the m_write_p
  */
  bool seek_write(unsigned int len);

  /* 
  * put_data - puts some data into the FIFO, no locking version 
  * Note that with only one concurrent reader and one concurrent 
  * writer, you don't need extra locking to use these functions. 
  */ 
  unsigned int kfifo_put(char* buffer, unsigned int len);

  /* 
  * get_data - gets some data from the FIFO, no locking version 
  * Note that with only one concurrent reader and one concurrent 
  * writer, you don't need extra locking to use these functions. 
  */ 
  unsigned int kfifo_get(char* buffer, unsigned int len);

protected:
  inline unsigned int _max(unsigned int a, unsigned int b)
  {
    return a > b ? a : b;
  }

  inline unsigned int _min(unsigned int a, unsigned int b)
  {
    return a < b ? a : b;
  }

  // 2^n
  inline bool is_power_of_2(unsigned int x) const
  {
    return (x != 0) && ((x & (x - 1)) == 0);
  }

private:
  int __buffer_size;
  char* __ring_buffer;
  unsigned int __write_p;
  unsigned int __read_p;
};

#endif /* __RING_BUFFER_H__ */
