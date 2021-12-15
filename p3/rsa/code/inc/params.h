#ifndef PARAMS_H
#define PARAMS_H

#include <stdlib.h>
#include <stdbool.h>
#include <linux/limits.h>
#include "logger.h"

#define DEFAULT_STRUCT_PARAMETERS \
  bool doTest; \
  char *outfPath;

#define INIT_DEFAULT_PARAMS_STRUCT( n ) \
  n.def.doTest = false;\
  n.def.outfPath = NULL;

typedef struct def_params {
  DEFAULT_STRUCT_PARAMETERS
} def_params_t;

typedef struct power_params {
  char* base;
  char* pow;
  char* modulus;
  struct def_params def;
} power_params_t;

typedef struct prime_params {
  char *bits;
  char *sec;
  bool min_test_suc;
  struct def_params def;
} prime_params_t;

power_params_t* params_parse_power( int argc, char **argv );
prime_params_t* params_parse_prime( int argc, char **argv );

#endif
