#include "ring_buffer.h"

ring_buffer::ring_buffer(int buf_size)
: __buffer_size(buf_size)
, __ring_buffer(NULL)
, __write_p(0)
, __read_p(0)
{
  __ring_buffer = new char[__buffer_size];

  assert(is_power_of_2(buf_size)); // size must be 2^n
}

ring_buffer::~ring_buffer()
{
  if (__ring_buffer)
    delete[] __ring_buffer;

  __ring_buffer = NULL;
}

unsigned int ring_buffer::kfifo_put(char* buffer, unsigned int len)
{
  if(len == 0)
    return 0;

  unsigned int iLen;
  len = _min(len, __buffer_size - __write_p + __read_p);

  /*
  * Ensure that we sample the fifo->out index -before- we
  * start putting bytes into the kfifo.
  */
  smp_mb();                                 /*===>B2*/ 

  /* first put the data starting from fifo->in to buffer end */
  iLen = _min(len, __buffer_size - (__write_p & (__buffer_size - 1)));
  memcpy(__ring_buffer + (__write_p & (__buffer_size - 1)), buffer, iLen);

  /* then put the rest (if any) at the beginning of the buffer */
  memcpy(__ring_buffer, buffer + iLen, len - iLen);

  /*
  * Ensure that we add the bytes to the kfifo -before-
  * we update the fifo->in index.
  */
  smp_wmb();                                 /*===>A1*/

  __write_p += len;

  return len;
}

unsigned int ring_buffer::peek(char* buffer, unsigned int len)
{
  if(empty() || used() < len)
    return 0;

  unsigned int iLen;
  len = _min(len, __write_p - __read_p);

  /*
  * Ensure that we sample the fifo->in index -before- we
  * start removing bytes from the kfifo.
  */
  smp_rmb();                   /*===>A2*/

  /* first get the data from fifo->out until the end of the buffer */
  iLen = _min(len, __buffer_size - (__read_p & (__buffer_size - 1)));
  memcpy(buffer, __ring_buffer + (__read_p & (__buffer_size - 1)), iLen);

  /* then get the rest (if any) from the beginning of the buffer */
  memcpy(buffer + iLen, __ring_buffer, len - iLen);

  /*
  * Ensure that we remove the bytes from the kfifo -before-
  * we update the fifo->out index.
  */
  smp_mb();                       /*===>B2*/

  //
  //// No move m_read ////

  return len;
}

bool ring_buffer::seekread(unsigned int len)
{
  if (used() < len)
    return false;

  __read_p += len;
  return true;
}

unsigned int ring_buffer::kfifo_get(char* buffer, unsigned int len)
{
  if(empty())
    return 0;

  unsigned int iLen;
  len = _min(len, __write_p - __read_p);

  /*
  * Ensure that we sample the fifo->in index -before- we
  * start removing bytes from the kfifo.
  */
  smp_rmb();                   /*===>A2*/

  /* first get the data from fifo->out until the end of the buffer */
  iLen = _min(len, __buffer_size - (__read_p & (__buffer_size - 1)));
  memcpy(buffer, __ring_buffer + (__read_p & (__buffer_size - 1)), iLen);

  /* then get the rest (if any) from the beginning of the buffer */
  memcpy(buffer + iLen, __ring_buffer, len - iLen);

  /*
  * Ensure that we remove the bytes from the kfifo -before-
  * we update the fifo->out index.
  */
  smp_mb();                       /*===>B2*/

  __read_p += len;
  return len;
}
