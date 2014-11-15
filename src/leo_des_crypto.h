#ifndef __LEO_DES_CRYPTO_H__
#define __LEO_DES_CRYPTO_H__

#include "leo_encrypt.h"

#define DEFAULT_ECB "1Z2X3C0"
#define DES_DIRMASK 001
#define DES_ENCRYPT (0 * DES_DIRMASK)
#define DES_DECRYPT (1 * DES_DIRMASK)

#define DES_DEVMASK 002
#define DES_HW (0 * DES_DEVMASK)
#define DES_SW (1 * DES_DEVMASK)

#define DESERR_NONE 0
#define DESERR_NOHWDEVICE 1
#define DESERR_HWERROR 2
#define DESERR_BADPARAM 3

#ifdef __cplusplus
extern "C" {
#endif

LEO_EXPORT int leo_ecb_crypt(const char* key, char* buf, unsigned len, unsigned mode);
LEO_EXPORT int leo_cbc_crypt(const char* key, char* buf, unsigned len, unsigned mode, char* ivec);

/* Not defined by Sun Microsystems - internally used by desbench program */
void leo_des_setkey(const char* key);
void leo_des_crypt(char* buf, int flag);

#ifdef __cplusplus
}
#endif

#endif /* __LEO_DES_CRYPTO_H__ */
