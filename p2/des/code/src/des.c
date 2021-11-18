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

#define BLOCKSZ 64

/* Constantes para el DES */
#define BITS_IN_PC1 56
#define BITS_IN_PC2 48
#define ROUNDS 16
#define BITS_IN_IP 64
#define BITS_IN_E 48
#define BITS_IN_P 32
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16

// #define callback_proto (void)(des_t*)
typedef des_error_t (*callback_proto)(des_t*);

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
  callback_proto operate; // run des with config previously set
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

/**
 * @brief Changes default des execution mode
 * 
 * @param des 
 * @param mode 
 */
void des_set_mode(des_t *des, des_mode_t mode);

/**
 * @brief returns callback reference given operation mode
 * 
 * @param mode mode used for switching
 * @return correspondant prototype
 */
callback_proto _get_callback(des_mode_t mode);

// mode operations -> ECB, CBC, CFB, OFB, CTR

des_error_t des_ecb(des_t*);
des_error_t des_cbc(des_t*);
des_error_t des_cfb(des_t*);
des_error_t des_ofb(des_t*);
des_error_t des_ctr(des_t*);
des_error_t des_nil(des_t*);

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

  // des->mode = mode;
  des->action = action;
  des->iv = iv;
  des->rounds = rounds;
  des->bitn = bitn;
  des->i_file = i_file;
  des->o_file = o_file;
  des->alphabet = NULL; // not using alphabet now, just ascii conversion

  des_set_mode(des, mode);

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
  default: // everything ok
    break;
  }
}

void des_set_mode(des_t *des, des_mode_t mode)
{
  if (des)
  {
    des->mode = mode;
    des->operate = _get_callback(des->mode);
  }
}

des_error_t des_execute(des_t *des)
{  
  if (!des || !des->operate)
    return BAD_ARG;

  return des->operate(des);
}

/* !! STATIC !! */

void build_parity_key(byte_ptr destkey, byte_ptr srckey)
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

callback_proto _get_callback(des_mode_t mode)
{
  // ECB, CBC, CFB, OFB, CTR
  
  switch (mode)
  {
  case ECB:
    return des_ecb;
    break;
  case CBC:
    return des_cbc;
    break;
  case CFB:
    return des_cfb;
    break;
  case OFB:
    return des_ofb;
    break;
  case CTR:
    return des_ctr;
    break;
  default:
    return des_nil;
    break;
  }
}

/** MODE OPERATIONS IMPLEMENTATIONS **/

des_error_t des_ecb(des_t* des)
{
  if (!des)
    return;

  // TODO
}

des_error_t des_cbc(des_t* des)
{
  if (!des)
    return;
  
  // TODO
}

des_error_t des_cfb(des_t* des)
{
  if (!des)
    return;

  printf("DES_ECB\n");
  
  // TODO
}

des_error_t des_ofb(des_t* des)
{
  if (!des)
    return;
  
  // TODO
}

des_error_t des_ctr(des_t* des)
{
  if (!des)
    return;
  
  // TODO
}

des_error_t des_nil(des_t *des){}

/** * * * * * * * TABLES * * * * * * * **/

/* "permutaci�n" PC1 */
static const unsigned short PC1[BITS_IN_PC1] = { 
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

/* "permutaci�n" PC2 */
static const unsigned short PC2[BITS_IN_PC2] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/* n�mero de bits que hay que rotar cada semiclave seg�n el n�mero de ronda */
static const unsigned short ROUND_SHIFTS[ROUNDS] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/* permutaci�n IP */
static const unsigned short IP[BITS_IN_IP] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

/* inversa de IP */
static const unsigned short IP_INV[BITS_IN_IP] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

/* expansi�n E */
static const unsigned short E[BITS_IN_E] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

/* permutaci�n P */
static const unsigned short P[BITS_IN_P] = {
	16, 7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2, 8, 24, 14,
	32, 27, 3, 9,
	19, 13, 30, 6,
	22, 11, 4, 25
};

/* cajas S */
static const unsigned short S_BOXES[NUM_S_BOXES][ROWS_PER_SBOX][COLUMNS_PER_SBOX] = {
	{	{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
		{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
		{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
		{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } 	},
	{
		{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
		{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
		{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
		{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }	},

	{	{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
		{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
		{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
		{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }	},

	{	{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
		{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
		{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
		{ 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }	},
	{
		{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
		{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
		{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
		{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }	},
	{	
		{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
		{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
		{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
		{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }	},
	{
 		{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
		{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
		{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
		{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }	},
	{
		{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
		{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
		{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
		{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }	}
};