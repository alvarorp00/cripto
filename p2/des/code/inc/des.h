/**
 * @file des.h
 * @author your name (you@domain.com)
 * @brief DES Algorithm header file
 * @version 0.1
 * @date 2021-11-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __DES_H__
#define __DES_H__

#include <stdio.h>
#include <stdint.h>

#include "util.h"

#define KLBYTES 8
#define KLBITS 64
#define BBITS 8
#define BITBLOCKSZ 64
#define BLBYTES 8

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


enum _des_action_t {CIPHER, DECIPHER}; // DES available actions

/**
 * @brief Posible
 * des errors
 * 
 */
enum _des_error_t {
  OK,
  INIT_ERROR,
  CONFIG_ERROR,
  LOAD_ERROR,
  BAD_ARG,
  BAD_PARITY,
  UNSUPP_MODE
}; // ERROR CODES

/**
 * @brief 3DES cipher
 * structure
 * 
 */
typedef struct _tdes_t tdes_t;

/**
 * @brief Single des
 * cipher structure
 * 
 */
typedef struct _des_t des_t;

/**
 * @brief struct
 * containing an state
 * of des process
 * 
 * (L, R)
 * 
 */
typedef struct _state_t state_t;

/**
 * @brief Defines
 * des action {CIPHER; DECIPHER}
 * 
 */
typedef enum _des_action_t des_action_t;

/**
 * @brief Structure
 * for handling des errors
 * 
 */
typedef enum _des_error_t des_error_t;

/* **************************** */
/* **************************** */
/* ************ DES *********** */
/* **************************** */
/* **************************** */

/**
 * @brief New des structure
 * 
 * @return des_t* 
 */
des_t *des_new();

/**
 * @brief Removes des
 * structure allocation
 * 
 * @param des to be removed
 */
void des_clean(des_t *des);

/**
 * @brief 
 * 
 * @param des 
 * @param mode 
 * @param action 
 * @param key 
 * @param iv 
 * @param sbit 
 * @param i_file 
 * @param o_file 
 * @return des_error_t 
 */
des_error_t des_configure
  (des_t *des, des_action_t action,
    dword key, dword iv, byte sbit, FILE *i_file, FILE *o_file);

/**
 * @brief runs cipher with previous config
 * with cfb operations
 * 
 * For further information
 * about how this works, please consider
 * visiting:
 * 
 * https://www.geeksforgeeks.org/block-cipher-modes-of-operation/
 * 
 * and see the cfb related topic
 * 
 * @param des structure
 * @return des_error_t 
 */
des_error_t des_cfb(des_t *des);

/**
 * @brief 
 * 
 * @param des 
 * @param msg 
 * @return des_error_t 
 */
des_error_t des_block(des_t *des, byte msg[]);

/**
 * @brief 
 * 
 * @param error code
 * @return error string
 */
const char *des_parse_error(des_error_t error);

/* **************************** */
/* **************************** */
/* *********** 3DES *********** */
/* **************************** */
/* **************************** */

/**
 * @brief Instantiates new
 * structure with TDEA
 * 
 * @return tdes_t* new tdes struct
 */
tdes_t *tdes_new();

/**
 * @brief Configures
 * TDEA with it's config
 * 
 * @param tdes struct
 * @param action action
 * @param keys 3 keys
 * @param iv initial vector
 * @param sbit shift bits
 * @param i_file input file
 * @param o_file output file
 * @return des_error_t possible error
 */
des_error_t tdes_configure
  (tdes_t *tdes, des_action_t action,
    dword keys[3], dword iv, byte sbit,
      FILE *i_file, FILE *o_file);

/**
 * @brief performs cipher
 * using CFB operation mode
 * 
 * @param tdes structure with config
 * @return des_error_t possible error
 */
des_error_t tdes_cfb(tdes_t* tdes);

/**
 * @brief Deletes tdes
 * associated resources
 * 
 * @param tdes struct to be cleaned
 */
void tdes_clean(tdes_t *tdes);


/* **************************** */
/* **************************** */
/* ********** TABLES ********** */
/* **************************** */
/* **************************** */

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

#endif