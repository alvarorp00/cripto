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

enum _des_mode_t {ECB, CBC, CFB, OFB, CTR}; // Just gives support to CFB
enum _des_action_t {CIPHER, DECIPHER, CRYPTANALYZE}; // DES available actions
enum _des_error_t {OK, INIT_ERROR, CONFIG_ERROR, LOAD_ERROR, BAD_ARG}; // ERROR CODES

typedef struct _des_t des_t;

typedef enum _des_mode_t des_mode_t;
typedef enum _des_action_t des_action_t;
typedef enum _des_error_t des_error_t;

/* PROTOTYPES */

des_t *des_new();

des_error_t des_configure
  (des_t *des, des_mode_t mode, des_action_t action, byte *key, byte *iv,
    byte rounds, byte bitn, FILE *i_file, FILE *o_file, const char* dictpath);

des_error_t des_execute(des_t *des);

void des_parse_error(des_error_t error);

#endif