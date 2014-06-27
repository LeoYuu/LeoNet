#ifndef __LEO_TPC_GAME_PROTOCOL_H__
#define __LEO_TPC_GAME_PROTOCOL_H__

#include <stdlib.h>
#include "leo_des_crypto.h"
#include "leo_dof_crypto.h"
#include "leo_protocols.h"
#include "leo_net_message.h"
#include "leo_dof_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

struct secret_key
{
  unsigned int session_key;
  char xor_key[XOR_KEY_LEN + 1];
  char swap_key[SWAP_KEY_LEN + 1];
};

struct about_crypt
{
  unsigned int sequence_id;
  unsigned int session_key;
  char xor_key[XOR_KEY_LEN + 1];
  char swap_key[SWAP_KEY_LEN + 1];
};

#pragma pack(pop)

LEO_EXPORT int send_key(struct about_crypt* ac, char* buf);

LEO_EXPORT int transform_message_to_buffer(struct about_crypt* ac, net_message* nm, char* buf, int len);

LEO_EXPORT int transform_buffer_to_message(struct about_crypt* ac, net_message* nm, char* buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __LEO_TPC_GAME_PROTOCOL_H__ */

