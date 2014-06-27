/*********************************************************************
* Filename:   des.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding DES implementation.
              Note that encryption and decryption are defined by how
              the key setup is performed, the actual en/de-cryption is
              performed by the same function.
*********************************************************************/

#ifndef DES_H
#define DESH

#include <stddef.h>

#define DES_BLOCK_SIZE 8

enum DES_MODE{
  DES_ENCRYPT,
  DES_DECRYPT
};

void des_key_setup(const unsigned char key[], unsigned char schedule[][6], DES_MODE mode);
void des_crypt(const unsigned char in[], unsigned char out[], const unsigned char key[][6]);

void three_des_key_setup(const unsigned char key[], unsigned char schedule[][16][6], DES_MODE mode);
void three_des_crypt(const unsigned char in[], unsigned char out[], const unsigned char key[][16][6]);

#endif   // DES_H
