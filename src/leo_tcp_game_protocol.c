#include "leo_tcp_game_protocol.h"

static void 
parse_to_message(const char* buf, int len, net_message* nm) {
  if(NULL == buf || len < 4) {
    assert(false);
    return;
  }

  unsigned short msg_id = *(unsigned short*)buf;
  unsigned short msg_len = *(unsigned short*)(buf + sizeof(unsigned short));

  assert(msg_len <= BUFFER_SIZE);

  nm->set_id(msg_id);
  nm->copy_data(buf + (2 * sizeof(unsigned short)), msg_len);
  nm->set_real_size(msg_len);
}

int
send_key(struct about_crypt* ac, char* buf) {
  unsigned int len;
  struct secret_key sk;

  ac->sequence_id = 0;
  ac->session_key = rand();
  memcpy(ac->xor_key, rand_xor_key(), XOR_KEY_LEN);
  ac->xor_key[XOR_KEY_LEN] = 0;
  ac->swap_key[SWAP_KEY_LEN] = 0;

  sk.session_key = ac->session_key;
  memcpy(sk.xor_key, ac->xor_key, sizeof(sk.xor_key));
  memcpy(sk.swap_key, ac->swap_key, sizeof(sk.swap_key));

  len = 0;
  *(unsigned short*)buf = sizeof(struct secret_key);
  len += sizeof(unsigned short);
  *(unsigned short*)(buf + len) = 0xffff;
  len += sizeof(unsigned short);
  memcpy(buf + len, &sk, sizeof(struct secret_key));
  len += sizeof(struct secret_key);

  leo_ecb_crypt(DEFAULT_ECB, buf, len, DES_ENCRYPT);

  return len + 2; /* des need */
}

/* len:id:size:data */
int 
transform_message_to_buffer(struct about_crypt* ac, net_message* nm, char* buf, unsigned int buf_len) {
  unsigned int len, data_size;

  data_size = nm->get_real_size();
  if(3 * sizeof(unsigned short) + data_size > buf_len) {
    return 0;
  }

  len = sizeof(unsigned short);
  *(unsigned short*)(buf + len) = nm->get_id();
  len += sizeof(unsigned short);
  *(unsigned short*)(buf + len) = data_size;
  len += sizeof(unsigned short);
  memcpy(buf + len, nm->get_data(), data_size);
  len += data_size;

  xor_crypt(buf + sizeof(unsigned short), len - sizeof(unsigned short), ac->xor_key);

  return len;
}

/* len:sequence:id:size:data:crc32 */
int
transform_buffer_to_message(struct about_crypt* ac, net_message* nm, char* buf, unsigned int msg_len) {
  unsigned int sequence_id, bp = 0;

  xor_decrypt(buf + sizeof(unsigned short), msg_len, ac->xor_key);
  if(!check_crc32(buf, sizeof(unsigned short) + msg_len)) {
    return 0;
  }

  bp += sizeof(unsigned short);
  msg_len -= sizeof(unsigned short);
  sequence_id = *(unsigned int*)(buf + bp);
  bp += sizeof(unsigned int);
  msg_len -= sizeof(unsigned int);

  if((sequence_id ^ ac->session_key) != ac->sequence_id) {
    return 0;
  }

  ac->sequence_id++;

  parse_to_message(buf + bp, msg_len, nm);

  return 0;
}

