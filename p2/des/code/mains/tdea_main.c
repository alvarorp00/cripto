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
  tdes_t *tdes        = NULL;
  des_action_t action = CIPHER; // by default, ciphers
  dword key[3]        = {0x00}; // 3keys
  dword iv            = 0x00;
  byte  sbit          = 0x00;

  byte i              = 0; // indexer for loop

  if (!(tdes = tdes_new()))
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
      sscanf(d_params->key, "%016lx%016lx%016lx", &key[0], &key[1], &key[2]);
      for (i=0;i<3;i++)
      {
        if (check_dword_parity(key[i]) == 0)
        {
          LOG_ERR("Bad parity for given key\n");
          exit(EXIT_FAILURE);
        }
        else if (key[i] == 0x00)
        {
          LOG_WARN("Using 0x00 as key\n");
        }
      }
    }
    action = DECIPHER;
  }
  else
  {
    for (i=0;i<3;i++)
    {
      key[i] = build_parity_key( get_random_key() );
    }
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

  LOG_INFO("Using hex key: %lx%lx%lx\n", key[0], key[1], key[2]);

  tdes_configure(tdes, action, key, iv, sbit, i_file, o_file);

  tdes_cfb(tdes);

  if (i_file != NULL && i_file != stdin)
    fclose(i_file);

  if (o_file != NULL && o_file != stdout)
    fclose(o_file);

  tdes_clean(tdes); // clean

  return 0;
}
