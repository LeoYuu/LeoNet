#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include "leo_utilities.h"

#if defined WIN32
# include <assert.h>
# include <windows.h>
# define mb() MemoryBarrier()
# define rmb() MemoryBarrier()
# define wmb() MemoryBarrier()
#elif defined __LINUX__
# define __ASM_FORM(x)  " " #x " "
# define __ASM_SEL(a,b) __ASM_FORM(a)

# define _ASM_ALIGN  __ASM_SEL(.balign 4, .balign 8)
# define _ASM_PTR  __ASM_SEL(.long, .quad)
# define __stringify_1(x...)  #x
# define __stringify(x...)  __stringify_1(x)

# define ALTERNATIVE(oldinstr, newinstr, feature)      \
  \
  "661:\n\t" oldinstr "\n662:\n"          \
  ".section .altinstructions,\"a\"\n"        \
  _ASM_ALIGN "\n"              \
  _ASM_PTR "661b\n"        /* label           */  \
  _ASM_PTR "663f\n"        /* new instruction */  \
  "   .byte " __stringify(feature) "\n"  /* feature bit     */  \
  "   .byte 662b-661b\n"      /* sourcelen       */  \
  "   .byte 664f-663f\n"      /* replacementlen  */  \
  ".previous\n"              \
  ".section .altinstr_replacement, \"ax\"\n"      \
  "663:\n\t" newinstr "\n664:\n"    /* replacement     */  \
  ".previous"

# define alternative(oldinstr, newinstr, feature) \
  asm volatile (ALTERNATIVE(oldinstr, newinstr, feature) : : : "memory")

# define X86_FEATURE_XMM    (0*32+25) /* "sse" */
# define X86_FEATURE_XMM2  (0*32+26) /* "sse2" */  

/*
* Force strict CPU ordering.
* And yes, this is required on UP too when we're talking
* to devices.
*/

//#if defined CONFIG_X86_32
/*
* Some non-Intel clones support out of order store. wmb() ceases to be a
* nop for these.
*/
//#define mb() alternative("lock; addl $0,0(%%esp)", "mfence", X86_FEATURE_XMM2)
//#define rmb() alternative("lock; addl $0,0(%%esp)", "lfence", X86_FEATURE_XMM2)
//#define wmb() alternative("lock; addl $0,0(%%esp)", "sfence", X86_FEATURE_XMM)
//#else
# define mb() asm volatile("mfence":::"memory")
# define rmb() asm volatile("lfence":::"memory")
# define wmb() asm volatile("sfence" ::: "memory")
//#endif // CONFIG_X86_32
#endif // #if defined __linux__

#define smp_mb() mb()
#define smp_rmb() rmb()
#define smp_wmb() wmb()

// ÎÞËø»º³å¶ÓÁÐ.
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
    return __buffer_size == used();
  }

  /*
  * Read data but don't move the m_read_p.
  */
  unsigned int peek(char* buffer, unsigned int len);
  /*
  * Move the m_read_p
  */
  bool seekread(unsigned int len);

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
