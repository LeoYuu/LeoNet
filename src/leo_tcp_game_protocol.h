#ifndef __LEO_TPC_GAME_PROTOCOL_H__
#define __LEO_TPC_GAME_PROTOCOL_H__

#include "leo_protocols.h"

#ifdef __cplusplus
extern "C" {
#endif

LEO_EXPORT int transform_message_to_buffer(net_message* nm, char* buf, int len);

LEO_EXPORT int transform_buffer_to_message();

#ifdef __cplusplus
}
#endif

#endif /* __LEO_TPC_GAME_PROTOCOL_H__ */


