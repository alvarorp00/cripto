#ifndef PARAMS_H
#define PARAMS_H

#include <stdlib.h>
#include <stdbool.h>
#include <linux/limits.h>
#include "logger.h"
#include "des.h"

#define DEFAULT_STRUCT_PARAMETERS \
  bool C, D; \
  char *infPath, *outfPath;

#define INIT_DEFAULT_PARAMS_STRUCT( n ) \
  n.def.C = false; \
  n.def.D = false; \
  n.def.infPath = NULL; \
  n.def.outfPath = NULL;

typedef struct def_params {
  DEFAULT_STRUCT_PARAMETERS
} def_params_t;

typedef struct des_params {
  char* key; // key used in decipher
  char* iv; // initialization vector
  byte sbit; // number of bits
  def_params_t def; // default params
} des_params_t;

des_params_t* params_parse_des( int argc, char **argv );

#endif
