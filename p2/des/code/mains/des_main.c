/**
 * @file des_main.c
 * @brief runs DES algorithm
 * @version 0.1
 * @date 2021-11-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "des.h"
#include "params.h"
#include "logger.h"
#include "util.h"

static FILE *i_file = NULL;
static FILE *o_file = NULL;

static des_params_t *d_params = NULL;

int main(int argc, char **argv)
{
  des_t *des          = NULL;
  des_action_t action = CIPHER; // by default, ciphers
  dword key           = 0x00;
  dword iv            = 0x00;
  byte  sbit          = 0x00;

  if (!(des = des_new()))
  {
    LOG_ERR("Failure initializing des: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  d_params = params_parse_des(argc, argv);

  if (!d_params)
  {
    LOG_ERR("Fatal error parsing params\n");
    exit(EXIT_FAILURE);
  }

  if (d_params->def.infPath == NULL)
  {
    i_file = stdin;
  }
  else
  {
    i_file = fopen(d_params->def.infPath, "rb");
    if (!i_file)
    {
      LOG_ERR("input stream couldn't be opened: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  if (d_params->def.outfPath == NULL)
  {
    o_file = stdout;
  }
  else
  {
    o_file = fopen(d_params->def.outfPath, "wb");
    if (!o_file)
    {
      LOG_ERR("output stream couldn't be opened: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  if (d_params->def.D)
  {
    if (d_params->key != NULL)
    {
      sscanf(d_params->key, "%lx", &key);
      if (check_dword_parity(key) == 0)
      {
        LOG_ERR("Bad parity for given key\n");
        exit(EXIT_FAILURE);
      }
      else if (key == 0x00)
      {
        LOG_WARN("Using 0x00 as key\n");
      }
    }
    action = DECIPHER;
  }
  else
  {
    key = build_parity_key( get_random_key() );
    action = CIPHER;
  }

  if (d_params->iv != NULL)
  {
    sscanf(d_params->iv, "%lx", &iv);
    if (iv == 0x00)
    {
      LOG_WARN("Using 0x00 as IV\n");
    }
  }
  
  sbit = d_params->sbit;

  if (sbit == 0 || (BITBLOCKSZ % sbit))
  {
    LOG_ERR("S bits to shift can't be 0 neither non-64 multiple\n");
    exit(EXIT_FAILURE);
  }

  // #define __PRINT_CONFIG
  #ifdef __PRINT_CONFIG
    printf("Key: %lx\n", key);
    printf("IV:  %lx\n", iv);
    printf("SBN: %x \n", sbit);
    printf("DES: %s \n", action ? "decipher" : "cipher");
  #endif

  LOG_INFO("Using hex key: %lx\n", key);

  des_configure(des, action, key, iv, sbit, i_file, o_file);

  des_cfb(des);

  if (i_file != NULL && i_file != stdin)
    fclose(i_file);

  if (o_file != NULL && o_file != stdout)
    fclose(o_file);

  return 0;
}
