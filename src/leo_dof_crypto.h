#ifndef __LEO_GAME_CRYPTO_H__
#define __LEO_GAME_CRYPTO_H__

#include "leo_encrypt.h"

#define true 1
#define false 0

#define SWAP_KEY_LEN 4
#define XOR_KEY_LEN 16
#define MAX_XOR_NUM 256

#define rand_between(a,b) ((a) + (rand() % (((b) - (a)) + 1)))

#ifdef __cplusplus
extern "C" {
#endif

LEO_EXPORT void initial_xor_keys();
LEO_EXPORT char* rand_xor_key();
LEO_EXPORT int xor_crypt(char* buf, unsigned int len, const char* key);
LEO_EXPORT int xor_decrypt(char* buf, unsigned int len, const char* key);

LEO_EXPORT void initial_crc32_table();
LEO_EXPORT unsigned int generate_crc32(const char* buf, unsigned int len);
LEO_EXPORT int check_crc32(const char* buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __LEO_GAME_CRYPTO_H__ */