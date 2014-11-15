#include "leo_net_session.h"

void server_session::recv_x2x_message_heart(const net_message* nm)
{
  printf("%s: recv message_heart.\n", __FUNCTION__);

  net_message* _send_msg = new net_message();
  if(_send_msg)
  {
    memcpy(_send_msg, nm, sizeof(net_message));

    send_message(_send_msg);
  }
  else
  {
    assert(false);
  }
}
