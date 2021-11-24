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

enum des_versions{DES, TDEA};

union des_selector
{
  tdes_t *tdes;
  des_t  *des;
};

// typedef des_error_t (*encrypt_proto)(union des_selector, dword, dword*);

/* STRUCT DEFINITION */

struct _tdes_t {
  des_action_t action; // action of des cipher
  des_t *des[3]; // each of 3 des ciphers
  union bconv_t keys[3]; // 3 keys
  dword iv; // initialization vector
  byte sbit; // bits to shift
  FILE *i_file; // input file
  FILE *o_file; // output file
};

struct _des_t {
  des_action_t action; // cipher, decipher, analyze
  union bconv_t key; // key used, 64b
  union bconv_t subk[ROUNDS]; // 16 keys of 48b each
  dword iv; // initialization vector
  byte sbit; // number of bits to shift in cfb
  FILE *i_file; // file to read from
  FILE *o_file; // file to dump to
};

struct _state_t {
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
 * @brief Performs cipher
 * over a 64b aligned block
 * using TDEA
 * 
 * @param tdes TDEA struct
 * @param msg to cipher
 * @param output 64b block cipher
 * @return des_error_t maybe an error
 */
des_error_t _tdes(tdes_t *tdes, dword msg, dword *output);

/**
 * @brief F function
 * 
 * @param r 32b input
 * @param sbk 48b key
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
void _IP(struct _state_t *state, dword msg);

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
void _IP_INV(struct _state_t *state, dword *output);

/**
 * @brief performs cfb operation
 * with given cipher (either des or tdea)
 * 
 * @param select des or tdes
 * @param version des or tdes flag
 * @return des_error_t possible error
 */
des_error_t _cfb( union des_selector select, enum des_versions version );

/* IMPLEMENTATIONS */

/* **************************** */
/* ************ DES *********** */
/* **************************** */

des_t *des_new()
{
  des_t *des = NULL;

  des = (des_t*) malloc(sizeof(des_t));

  return des;
}

des_error_t des_configure
  (des_t *des, des_action_t action,
    dword key, dword iv, byte sbit, FILE *i_file, FILE *o_file)
{
  if (!des || !i_file || !o_file)
    return BAD_ARG;

  /* Common configs */
  des->key.l = key;
  if (check_dword_parity(des->key.l) == 0)
    return BAD_PARITY;
  des->action = action;
  des->iv = iv;
  des->sbit = (BITBLOCKSZ % sbit) ? 8 : sbit; // if !(sbit|64) use 8-CFB 
  des->i_file = i_file;
  des->o_file = o_file;

  return OK;
}

void des_clean(des_t *des){if (des) free(des); }

/* **************************** */
/* *********** 3DES *********** */
/* **************************** */

tdes_t *tdes_new()
{
  tdes_t *tdes = NULL;

  tdes = (tdes_t*)malloc(sizeof(tdes_t));

  return tdes;
}

des_error_t tdes_configure
  (tdes_t *tdes, des_action_t action,
    dword keys[3], dword iv, byte sbit,
      FILE *i_file, FILE *o_file)
{
  des_error_t err;
  byte i;
  
  if (!tdes || !i_file || !o_file)
    return BAD_ARG;
  
  for (i=0; i<3; i++)
  {
    tdes->des[i] = (des_t*)malloc(sizeof(des_t));
    if (!tdes->des[i])
      return INIT_ERROR;
    err = des_configure(tdes->des[i], action,
      keys[i], iv, sbit, i_file, o_file); // some args won't be used
    if (err)
      return err;
    tdes->keys[i].l = keys[i];
  }

  //manually...
  tdes->des[1]->action = (action == CIPHER) ? DECIPHER : CIPHER; //invert

  tdes->action = action;
  tdes->iv     = iv;
  tdes->sbit   = sbit;
  tdes->i_file = i_file;
  tdes->o_file = o_file;
  
  return OK;
}

void tdes_clean(tdes_t *tdes)
{
  if (tdes)
  {
    des_clean(tdes->des[0]);
    des_clean(tdes->des[1]);
    des_clean(tdes->des[2]);
    free(tdes);
  }
}


/* ERROR PARSER */

const char *des_errors[7] = {
  "everything ok",
  "error while initializing resource",
  "error while configurating des",
  "error while loading resource",
  "error due to bad argument",
  "error due to bad parity found",
  "error due to unsupported required mode"
};
const char *des_parse_error(des_error_t error){ return des_errors[error]; }

/** ******************************* **/
/** MODE OPERATIONS IMPLEMENTATIONS **/
/** ******************************* **/

des_error_t des_cfb(des_t* des)
{
  union des_selector selector;
  
  if (!des)
    return BAD_ARG;

  selector.des = des;
  return _cfb(selector, DES);
}

des_error_t tdes_cfb(tdes_t* tdes)
{
  union des_selector selector;
  
  if (!tdes)
    return BAD_ARG;

  selector.tdes = tdes;
  return _cfb(selector, TDEA);
}

des_error_t _cfb( union des_selector select, enum des_versions version )
{
  des_t   *des        = NULL;
  tdes_t  *tdes       = NULL;
  
  dword block         = 0; // cipher block
  dword rd            = 0; // data read

  dword shift_reg     = 0; // shift register
  dword ctb           = 0; // partial cipher text block
  dword ptb           = 0; // partial plain text block
  byte  bround        = 0; // bits read in round 

  size_t n            = 0; // check fread return

  des_action_t action = 0; // cfb action
  dword iv            = 0; // init. vector
  byte sbit           = 0; // shift bits
  FILE *ifile         = NULL; //input file
  FILE *ofile         = NULL; // output file

  if (version == DES)
  {
    des = select.des;
    if (!des)
      return BAD_ARG;

    // config...
    action = des->action;
    des->action = CIPHER; // always use des encryption
    
    iv = des->iv;
    sbit = des->sbit;

    ifile = des->i_file;
    ofile = des->o_file;
  }
  else /* if version == TDEA */ 
  {
    tdes = select.tdes;
    if (!tdes)
      return BAD_ARG;

    // config...
    action = tdes->action;
    tdes->action = CIPHER;

    iv = tdes->iv;
    sbit = tdes->sbit;

    ifile = tdes->i_file;
    ofile = tdes->o_file;
  }

  shift_reg = iv;

  if (ifile == stdin)
  {
    printf("-> Enter message (press CTRL + D in new line to finish): \n");
  }

  while ( !feof( ifile ) )
  {
    n = fread( &rd, (sbit / 8), 1, ifile );
    
    if (n == 0)
      break;

    if ( action == CIPHER )
    {
      // _des(des, shift_reg, &(shift_reg));
      if (version == DES)
        _des(des, shift_reg, &(shift_reg));
      else
        _tdes(tdes, shift_reg, &(shift_reg));
      ctb = rd ^ (shift_reg >> (64 - sbit));
      shift_reg <<= sbit;
      shift_reg |= ctb;
      block |= (ctb << (bround));
    }
    else /* if des->action == DECIPHER */
    {
      // _des(des, shift_reg, &(shift_reg));
      if (version == DES)
        _des(des, shift_reg, &(shift_reg));
      else
        _tdes(tdes, shift_reg, &(shift_reg));
      ptb = rd ^ (shift_reg >> (64 - sbit));
      shift_reg <<= sbit;
      shift_reg |= rd;
      block |= (ptb << (bround));
    }

    bround = (bround + sbit);

    if ((bround % BITBLOCKSZ) == 0) // fits well
    {
      fwrite(&block, sizeof(dword), 1, ofile); // print block
      block = 0;
      bround = 0;
    }
  }

  if (bround > 0) // pad ?
    fwrite(&block, bround / 8, 1, ofile); // print remaining...
  
  return OK;
}

/* **************************** */
/* **************************** */
/* ********* STATIC *********** */
/* **************************** */
/* **************************** */

 des_error_t _des(des_t *des, dword msg, dword *block)
{  
  struct _state_t state;
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

des_error_t _tdes(tdes_t *tdes, dword msg, dword *block)
{
  if (!tdes)
    return BAD_ARG;

  if (tdes->action == CIPHER)
  {
    tdes->des[0]->action = CIPHER;
    tdes->des[1]->action = DECIPHER;
    tdes->des[2]->action = CIPHER;
    
    _des(tdes->des[0], msg, &(msg));
    _des(tdes->des[1], msg, &(msg));
    _des(tdes->des[2], msg, block);
  }
  else
  {
    tdes->des[2]->action = DECIPHER;
    tdes->des[1]->action = CIPHER;
    tdes->des[0]->action = DECIPHER;
    
    _des(tdes->des[2], msg, &(msg));
    _des(tdes->des[0], msg, &(msg));
    _des(tdes->des[1], msg, block);
  }

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

void _IP(struct _state_t *state, dword msg)
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

void _IP_INV(struct _state_t *state, dword *output)
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

  for (i=0; i<ROUNDS; i++)
  {
    c = (SPLITKMASK) & CLSHIFT(c, ROUND_SHIFTS[i], 28);
    d = (SPLITKMASK) & CLSHIFT(d, ROUND_SHIFTS[i], 28);

    _key48 = (((dword) c) << 28 | d);

    des->subk[i].l = 0;
    for (j=0; j<BITS_IN_PC2; j++)
    {
      des->subk[i].l <<= 1;
      des->subk[i].l |= (_key48 >> (56 - PC2[j])) & 1;
    }
  } // keys generated
}