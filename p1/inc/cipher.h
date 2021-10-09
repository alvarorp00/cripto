#ifndef __CIPHER_H__
#define __CIPHER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "gmp.h"

enum OPTION {
  CIPHER, DECIPHER
};

void affine(
  enum OPTION opt,
  const char *m,
  const char *a,
  const char *b,
  FILE *i_file,
  FILE *o_file
);

#define ERRBUFF_LEN 1024

extern char errbuff[ERRBUFF_LEN + 1];
extern bool cipher_status;

#endif