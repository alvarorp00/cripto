/**
 * @file des.c
 * @author Álvaro Rodríguez (alvarorp00@sigsuspend.net)
 * @brief DES algorithm
 * @version 0.1
 * @date 2021-11-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "des.h"
#include "alphabet.h"

#define KEY_LENGTH_BYTES 8
#define KEY_LENGTH_BITS 64
#define IVL 8 // size in bytes -> 8bits

#define BITCONV 8 // 1 byte <-> 8 bits

/* STRUCT DEFINITION */

struct _des_t {
  des_mode_t mode; // only supports CBF for now...
  des_action_t action; // cipher, decipher, analyze
  byte *key; // key used in byte level -> 8 bytes
  byte *iv; // initialization vector
  byte rounds; // number of rounds des will run
  byte bitn; // number of bits
  FILE *i_file; // file to read from
  FILE *o_file; // file to dump to
  alphabet_t *alphabet; // alphabet to translate
};

/* STATIC PROTOTYPES USED */

/**
 * @brief Sets new key in 64 bits
 * from one in 56 bits.
 * 
 * @param destkey destination key in 64 bits
 * @param srckey source key in 56 bits
 */
void _build_parity_key(byte_ptr destkey, byte_ptr srckey);

/* IMPLEMENTATIONS */

des_t *des_new()
{
  des_t *des = NULL;

  des = (des_t*) malloc(sizeof(des_t));

  return des;
}

des_error_t des_configure
  (des_t *des, des_mode_t mode, des_action_t action, byte *key, byte *iv,
    byte rounds, byte bitn, FILE *i_file, FILE *o_file, const char* dictpath)
{
  des_error_t err = OK;
  
  if (!des || !key || !i_file || !o_file)
    return BAD_ARG;

  /* Common configs */

  des->key = (byte*)calloc(KEY_LENGTH_BYTES, sizeof(byte));
  if (!des->key)
    return INIT_ERROR;

  des->mode = mode;
  des->action = action;
  des->iv = iv;
  des->rounds = rounds;
  des->bitn = bitn;
  des->i_file = i_file;
  des->o_file = o_file;
  des->alphabet = NULL; // not using alphabet now, just ascii conversion

  /* Load key */

  _build_parity_key(des->key, key);

  return err;
}

void des_parse_error(des_error_t error)
{
  switch (error)
  {
  case INIT_ERROR:
    /* code */
    break;
  case CONFIG_ERROR:
    break;
  case LOAD_ERROR:
    break;
  default: // OK
    break;
  }
}

/* !! STATIC !! */

void _build_parity_key(byte_ptr destkey, byte_ptr srckey)
{
  size_t i, j, cutter;

  if (!destkey || !srckey)
    return;
    
  for (i=0; i<KEY_LENGTH_BYTES; i++) // for each part of the given key (8 blocks of 7b)
  {
    destkey[i] = 0;
    for (j=0, cutter=0x01; j<BITCONV - 1; j++, cutter<<=1) // shift 1b every round
      destkey[i] |= (cutter & srckey[i]);
  destkey[i] |= 0; // i-th block of the key gets a zero-bit, redundant but explicit
  }
}