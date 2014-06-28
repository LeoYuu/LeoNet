#include "leo_dof_crypto.h"

#include <stdlib.h>

static unsigned int crc32_table[0xFF];
static unsigned int multinomial = 0x04c11db7;
static char xor_keys[MAX_XOR_NUM][XOR_KEY_LEN + 1];

void 
initial_xor_keys() {
  int i, j;
  for(i = 0; i < MAX_XOR_NUM; i++) {
    for(j = 0; j < XOR_KEY_LEN; j++) {
      xor_keys[i][j] = (char)rand_between(1, 255);
    }
    xor_keys[i][16] = 0;
  }
}

char*
rand_xor_key() {
  return xor_keys[(unsigned int)rand() % MAX_XOR_NUM];
}

int
xor_crypt(char* buf, unsigned int len, const char* key) {
  int *_key, *_buf;
  char* tmp_buf;
  const char* tmp_key;
  unsigned int sections, remains, i, j;

  j = 0;
  _key = (int*)key;
  _buf = (int*)buf;
  sections = len / sizeof(int);
  remains = len % sizeof(int);

  for(i = 0; i < sections; ++i) {
    *_buf = (*_buf) ^ _key[j];
    j = (j + 1) % sizeof(int);
    _buf++;
  }

  if(remains > 0) {
    tmp_key = key;
    tmp_buf = &buf[len - remains];
    for(i = 0; i < remains; ++i) {
      *tmp_buf = (*tmp_buf) ^ (*tmp_key);
      tmp_buf++;
      tmp_key++;
    }
  }

  return true;
}

int 
xor_decrypt(char* buf, unsigned int len, const char* key) {
  return xor_crypt(buf, len, key);
}

static unsigned int
reflect(unsigned int l, char c) {
  int i;
  unsigned int val = 0;

  for(i = 1; i < (c + 1); i++) { 
    if(l & 1) {
      val |= 1U << (c - i);
    }
    l >>= 1; 
  }

  return val;
}

void
initial_crc32_table() { 
  unsigned int i, j, t, t1, t2, flag;

  for(i = 0; i <= 0xFF; i++) { 
    t = reflect(i, 8);
    crc32_table[i] = t << 24;
    for(j = 0; j < 8; j++) {
      t1 = crc32_table[i] << 1;
      flag = crc32_table[i] & 0x80000000;
      if(0 == flag) {
        t2 = 0;
      } else {
        t2 = multinomial;
      }
      crc32_table[i] = t1 ^ t2 ; 
    }
    crc32_table[i] = reflect(crc32_table[i], 32);
  }
}

unsigned int
generate_crc32(const char* buf, unsigned int len) {
  unsigned char c, t;
  unsigned int crc32, old_crc, old_crc32, counts;

  counts = 0;
  old_crc32 = 0x00000000; 

  while(len) {
    t = (unsigned char)((old_crc32 >> 24) & 0x000000FF);
    old_crc = crc32_table[t]; 
    c = (unsigned char)buf[counts];
    old_crc32 = (old_crc32 << 8) | (c & 0X000000FF); 
    old_crc32 = old_crc32 ^ old_crc;
    counts++;
    len--;
  }
  crc32 = old_crc32;

  return crc32;
}

int 
check_crc32(const char* buf, unsigned int len) {
  unsigned int crc, new_crc;

  crc  = *(unsigned int*)&buf[len - sizeof(unsigned int)];
  new_crc = generate_crc32(buf, (len - 4));

  return (crc == new_crc);
}
