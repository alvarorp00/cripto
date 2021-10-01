/**
 * @file cipher.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-09-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "gmp.h"
#include "cipher.h"
#include "calclib.h"

enum OPTION {
  CIPHER, DECIPHER
};

/**
 * @brief Helper function for affine
 * 
 */
static void affine_cipher(
  uint_fast64_t m,
  uint_fast64_t a,
  uint_fast64_t b,
  const char *i_file
);

/**
 * @brief Helper function for affine
 * 
 */
static void affine_decipher(
  uint_fast64_t m,
  uint_fast64_t a,
  uint_fast64_t b,
  const char *o_file
);

/* - - - - - - - - !! CODE !! - - - - - - - - */



/**
 * @brief Affine cipher using
 * numerical procedures of multiple precision
 * defined in GNU MP (GMP).
 * 
 * Verifies that both numbers given, a & b,
 * determine an affine inyective function.
 * If they not, it'll raise an error.
 * 
 * If input file is not given, standard input will be held until
 * data is provided.
 * 
 * If output file is not given, standard output will be used.
 * 
 */
void affine(
  enum OPTION opt,
  uint_fast64_t m,
  uint_fast64_t a,
  uint_fast64_t b,
  const char *i_file,
  const char *o_file
)
{
  
}

static void affine_cipher(
  uint_fast64_t m,
  uint_fast64_t a,
  uint_fast64_t b,
  const char *i_file
)
{

}

static void affine_decipher(
  uint_fast64_t m,
  uint_fast64_t a,
  uint_fast64_t b,
  const char *o_file
)
{

}