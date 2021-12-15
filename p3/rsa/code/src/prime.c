/**
 * @file prime.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "prime.h"
#include "power.h"
#include "sodium.h"

struct _prime_gen_t
{
  size_t bits; // up to 9223372036854775807L bits!
  double err_level; // error margin
  FILE *o_file; // dumper of the program
};

/* ############################################################ */
/* ############################################################ */
/* ####################### PROTOS ############################# */
/* ############################################################ */
/* ############################################################ */

static void _nRandombits(mpz_t store_here, size_t bits_n);

/* ############################################################ */
/* ############################################################ */
/* ####################### PUBLIC ############################# */
/* ############################################################ */
/* ############################################################ */

prime_gen_t *prime_generator_init()
{
  return (prime_gen_t*)malloc(sizeof(prime_gen_t)); // lazy init
}

prime_gen_err_t prime_generator_configure(prime_gen_t *generator, size_t bits, double err_level, FILE *o_file)
{
  if (!generator) return NOT_INITIALIZED;

  if (!o_file) return NO_ARGUMENT_GIVEN;

  generator->bits = bits;
  generator->err_level = err_level;
  generator->o_file = o_file;

  return OP_OK;
}

prime_gen_err_t prime_generator_generate(prime_gen_t *generator, mpz_t res)
{

  mpz_t candidate;
  size_t required_candidates;

  double err_lesser_than = 1; // max

  power_t *power;
  mpz_power_error_t pw_err;

  size_t i;
  
  if (!generator) return NOT_INITIALIZED;

  mpz_init(candidate);

  // get a prime candidate
  _nRandombits(candidate, generator->bits);

  power = power_init();
  if (!power) return INIT_FAILURE;

  for (i=0; err_lesser_than >= generator->err_level; i++)
    err_lesser_than = (1 / pow(4, i));
  required_candidates = i;

  // printf("Err level: %lf\n\t Bases required: %ld\n", err_lesser_than, required_candidates);

  // TODO

  power_free(power);
  mpz_clear(candidate);
  
  return OP_OK;
}

prime_gen_err_t prime_generator_clean(prime_gen_t *generator)
{
  if (!generator) return NOT_INITIALIZED;

  free(generator);

  return OP_OK;
}

/* ############################################################ */
/* ############################################################ */
/* ###################### PRIVATE ############################# */
/* ############################################################ */
/* ############################################################ */

static void _nRandombits(mpz_t store_here, size_t bits_n)
{
  uint32_t _MSB_UP;

  uint32_t mask = 0x0;
  uint32_t excd; // excedent
  uint32_t _nbuf; // numbered buffer
  
  size_t i, blocks;

  // number of bits that must be preserved
  excd = (uint32_t) (bits_n % 32);
  blocks = (bits_n / 32); // blocks apart from last one

  // build final mask if 32b does not fit bits_ns
  for (i=0; i<excd; i++)
  {
    mask <<= 1;
    mask |=  1;
  }
  if (excd)
    _MSB_UP = (1 << (excd - 1));
  else
    _MSB_UP = 0;

  _nbuf = randombytes_random();
  _nbuf |= _MSB_UP;
  _nbuf &= mask;
  mpz_set_ui(store_here, _nbuf);

  // first block manually in case...
  if (blocks > 0)
  {
    if (excd)
      mpz_mul_2exp(store_here, store_here, 32);
    mpz_add_ui(store_here, store_here, (long)(0x80000000 | randombytes_random()));

    for (i=1; i<blocks; i++)
    {
      mpz_mul_2exp(store_here, store_here, 32);
      mpz_add_ui(store_here, store_here, (long)(randombytes_random()));
    } // candidate generated
  }

  if (mpz_even_p(store_here) == 0)
    mpz_add_ui(store_here, store_here, 1L); // now we have an even number

  if (mpz_cmp_ui(store_here, 2L) < 0)
    _nRandombits(store_here, bits_n); // must be bigger than 2!!!
}

// static bool _isPrime()
// {

// }