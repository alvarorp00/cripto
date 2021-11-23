/**
 * @file des.c
 * @brief DES algorithm
 * @version 0.1
 * @date 2021-11-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "des.h"

// #define callback_proto (void)(des_t*)
typedef des_error_t (*callback_proto)(des_t*);

/* STRUCT DEFINITION */

struct _des_t {
  des_action_t action; // cipher, decipher, analyze
  union bconv_t key; // key used, 64b
  union bconv_t subk[ROUNDS]; // 16 keys of 48b each
  dword iv; // initialization vector
  byte sbit; // number of bits
  FILE *i_file; // file to read from
  FILE *o_file; // file to dump to
};

struct state_t {
  word l;
  word r;
};

/* STATIC PROTOTYPES USED */


/**
 * @brief Performs cipher
 * over a 64b aligned block
 * 
 * @param des struc
 * @param msg to cipher
 * @param output 64b block cipher
 * @return des_error_t maybe an error
 */
des_error_t _des(des_t *des, dword msg, dword *output);

/**
 * @brief F function
 * 
 * @param r 32b
 * @param sbk 48b
 * @return word 32b output
 */
word _f(word r, union bconv_t sbk);

/**
 * @brief Selects key to be used
 * depending on the round and on
 * the des mode (CIPHER or DECIPHER)
 * 
 * @param des structure
 * @param round 
 * @return selected key from des.sbkey
 */
union bconv_t _key_selector(des_t *des, byte round);

/**
 * @brief Expands key into
 * 16 subkeys of 48 bits each
 * 
 * @param des structure
 */
void _key_expansion(des_t *des);

/**
 * @brief Performs initial
 * permutation of block
 * 
 * @param state state
 * @param msg message
 */
void _IP(struct state_t *state, dword msg);

/**
 * @brief Performs
 * final inverse permutation
 * of block
 * 
 * @param state as comes from
 * previous operations, with no
 * swap done yet
 * @param output final output
 */
void _IP_INV(struct state_t *state, dword *output);

/* IMPLEMENTATIONS */

des_t *des_new()
{
  des_t *des = NULL;

  des = (des_t*) malloc(sizeof(des_t));

  return des;
}

des_error_t des_configure
  (des_t *des, des_action_t action,
    dword key, dword iv, dword sbit, FILE *i_file, FILE *o_file)
{
  if (!des || !i_file || !o_file)
    return BAD_ARG;

  /* Common configs */
  des->key.l = key; // assume key is already parity builded
  des->action = action;
  des->iv = iv;
  des->sbit = sbit;
  des->i_file = i_file;
  des->o_file = o_file;

  return OK;
}

/* !! STATIC !! */

des_error_t _des(des_t *des, dword msg, dword *block)
{  
  struct state_t state;
  union bconv_t k;
  word l, r, _r;
  byte i;
  
  if (!des || !msg)
    return BAD_ARG; 

  _key_expansion(des);
  _IP(&state, msg);

  l = state.l;
  r = state.r;
  for (i=0; i<ROUNDS; i++)
  {
    k = _key_selector(des, i);
    _r = r;
    r  = l ^ _f(r, k);;
    l  = _r;
  }

  state.l = l;
  state.r = r;

  _IP_INV(&state, block);
  
  return OK;
}

union bconv_t _key_selector(des_t *des, byte round)
{
  union bconv_t k = {0x00};
  
  if (!des)
    return k;

  if (des->action == CIPHER)
    k = des->subk[round];
  else /* if (des ->mode == DECIPHER) */
    k = des->subk[(ROUNDS-1) - round];
  return k;
}

word _f(word r, union bconv_t sbk)
{
  dword ker;
  dword er;
  dword k;

  word  s;
  word  rs;
  
  byte  i;
  byte  b;
  
  byte  row;
  byte  column;

  k = sbk.l; // using 48b full representation

  er = 0;
  for (i=0; i<BITS_IN_E; i++)
  {
    er <<= 1;
    er |= (dword) ((r >> (32 - E[i])) & 1);
  }

  ker = k ^ er; // 48b expanded key, 8blocks of 6bit

  #define MASK_1_6 0x0000840000000000 /* 1st and 6th bit */
  #define MASK_2_5 0x0000780000000000 /* 2th - 5th bit */

  s = 0;
  for (i=0; i<NUM_S_BOXES; i++)
  {
    b      = (byte)((ker & (MASK_1_6 >> 6*i)) >> (42 - 6*i));
    row    = ((b >> 4) | (b & 0x01)); /* get [0,1,2,3] as index */

    /* last shift (43 but no 42) because now we have 4 bits aligned */
    column = (byte)(((ker & (MASK_2_5 >> 6*i)) >> (43 - 6*i)));

    s      <<= 4; /* each iteration causes 4 bit shift */
    s      |= (word)(S_BOXES[i][row][column] & 0x0F);
  }

  rs = 0;
  for (i=0; i<BITS_IN_P; i++)
  {
    rs <<= 1;
    rs |= (s >> (32 - P[i])) & 1;
  }
  
  return rs;
}

void _IP(struct state_t *state, dword msg)
{
  // union bconv_t conversion;
  dword _ip;
  int32_t i;

  if (!state || !msg)
    return;

  _ip = 0;
  for (i=0; i<BITS_IN_IP; i++)
  {
    _ip <<= 1;
    _ip |= (msg >> (BITS_IN_IP - IP[i])) & 1;
  }

  #define BMASK32 0x00000000FFFFFFFF

  state->l = (word)(_ip >> 32) & BMASK32;
  state->r = (word)(_ip) & BMASK32;
}

void _IP_INV(struct state_t *state, dword *output)
{
  dword blck;
  dword inv_ip;

  byte i;

  blck = ((((dword)state->r) << 32) | (dword) state->l);

  inv_ip = 0;
  for (i=0; i<BITS_IN_IP; i++)
  {
    inv_ip <<= 1;
    inv_ip |= (blck >> (BITS_IN_IP - IP_INV[i] )) & 1;
  }

  *(output) = inv_ip;
}

void _key_expansion(des_t *des) // 64b; 16 keys of 48bits
{
  dword _key64, _key56, _key48;
  word c, d;
  byte i, j;

  if (!des)
    return; // exit

  _key64 = des->key.l;

  for (i=0; i<BITS_IN_PC1; i++)
  {
    _key56 <<= 1;
    _key56 |= (_key64 >> (KLBITS - PC1[i])) & 1;
  }

  #define SPLITKMASK 0x0000000FFFFFFF

  c = (word)(_key56 >> 28) & SPLITKMASK;
  d = (word) _key56 & SPLITKMASK;

  #define CSHIFT(x,l,s) ((x<<l) | (x>> (s -l)))
  // #define CSHIFT(x,l,s) ((MASK28B&(x<<l)) | ((x >> (s-l)) & l))

  for (i=0; i<ROUNDS; i++)
  {
    c = (SPLITKMASK) & CSHIFT(c, ROUND_SHIFTS[i], 28);
    d = (SPLITKMASK) & CSHIFT(d, ROUND_SHIFTS[i], 28);

    _key48 = (((dword) c) << 28 | d);

    des->subk[i].l = 0;
    for (j=0; j<BITS_IN_PC2; j++)
    {
      des->subk[i].l <<= 1;
      des->subk[i].l |= (_key48 >> (56 - PC2[j])) & 1;
    }
  } // keys generated
}


/* ERROR PARSER */

void des_parse_error(des_error_t error, char errbuff[static 128])
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
  default: // everything ok
    break;
  }
}

/** MODE OPERATIONS IMPLEMENTATIONS **/

des_error_t des_cfb(des_t* des)
{
  dword rd;        // data read
  dword block = 0; // cipher block

  dword shift_reg    = 0; // shift register
  
  byte i;

  if (!des)
    return BAD_ARG;

  shift_reg = des->iv;

  if (des->i_file == stdin)
  {
    printf("-> Enter message [8B] (press CTRL + D in new line to finish): \n");
  }

  while ( !feof( des->i_file ) )
  {
    // padding is already done in this way
    size_t n = fread( &rd, sizeof(dword), 1, des->i_file );

    if ( des->action == CIPHER )
    {
      _des(des, shift_reg, &(block));
      block >>= (64 - des->sbit);
      block ^= rd;
      shift_reg = (block >> des->sbit);
    }
    else /* if des.action == DECIPHER*/
    {
      _des(des, shift_reg, &(block));
      block >>= (64 - des->sbit);
      block ^= rd;
      shift_reg = (rd >> des->sbit);
    }

    fwrite(&block, sizeof(dword), 1, des->o_file);
  }
  
  return OK;
}