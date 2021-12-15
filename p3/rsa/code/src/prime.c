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
#include <math.h>

#include "prime.h"
#include "sodium.h"
#include "logger.h"

struct _prime_gen_t
{
  size_t bits; // up to 9223372036854775807L bits!
  double err_level; // error margin
  double err_final_level; // error margin calculated
  FILE *o_file; // dumper of the program
};

/* ############################################################ */
/* ############################################################ */
/* ####################### PROTOS ############################# */
/* ############################################################ */
/* ############################################################ */

static void _nRandombits(mpz_t store_here, size_t bits_n);
static bool _isPrime(mpz_t candidate, size_t required_rounds);
static bool _miller_rabin_test(mpz_t d, mpz_t candidate);

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

prime_gen_err_t prime_generator_generate(prime_gen_t *generator, struct prime_number_guess *guess)
{

  mpz_t candidate;

  double err_lesser_than = 1; // max
  size_t bases_required;

  size_t i;
  
  if (!generator) return NOT_INITIALIZED;
  if (!guess)     return NO_ARGUMENT_GIVEN;

  mpz_init(candidate);

  // get a prime candidate
  _nRandombits(candidate, generator->bits); // candidate := n (random odd number)

  for (i=0; err_lesser_than >= generator->err_level; i++)
    err_lesser_than = (1 / pow(4, i));
  bases_required = i;
  generator->err_final_level = err_lesser_than;

  #ifdef __VERBOSE
    LOG_INFO("Err level: %lf\n\t Bases required: %ld\n\n", err_lesser_than, bases_required);
  #endif

  if (!_isPrime(candidate, bases_required))
  {
    mpz_clear(candidate);
    guess->is_prime = false;
    guess->prob_of_prime = 0.0f;
  }
  else
  {
    mpz_set(guess->candidate, candidate);
    guess->is_prime = true;
    guess->prob_of_prime = (1 - generator->err_final_level);
    mpz_clear(candidate);
  }
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

  if (mpz_odd_p(store_here) == 0)
    mpz_add_ui(store_here, store_here, 1L); // now we have an odd number

  if (mpz_cmp_ui(store_here, 3L) < 0)
    _nRandombits(store_here, bits_n); // must be bigger than 3 at least!!!
}

static bool _isPrime(mpz_t candidate, size_t required_rounds)
{
  mpz_t _d, _x, _aux;
  size_t i;

  bool _p_flag = false; // false -> not prime; true -> prime
  
  mpz_inits(_d, _x, _aux, NULL);

  mpz_sub_ui(_d, candidate, 1L); // d := n - 1;

  while (true)
  {
    mpz_mod_ui(_aux, _d, 2L);
    if (mpz_sgn(_aux) != 0)
      break;
    mpz_div_2exp(_d, _d, 1); // d >>= 1;
  }; // found an r such n = 2^d * r + 1 for some r >= 1

  for (i=0; i<required_rounds; i++)
    if (!_miller_rabin_test(_d, candidate))
      break;

  if (i==required_rounds) { _p_flag = true; } // all tests passed !

  mpz_clears(_d, _x, _aux, NULL);

  return _p_flag;
}

static bool _miller_rabin_test(mpz_t d, mpz_t candidate)
{
  mpz_t _d, _x, _random_bounded, _aux;
  gmp_randstate_t state;

  bool _p_flag = false;

  mpz_inits(_d, _x, _random_bounded, _aux, NULL);
  gmp_randinit_mt(state); // Mersenne Twister algorithm
  
  mpz_set(_d, d); // don't modify values

  mpz_sub_ui(_aux, candidate, 4L); // _aux := n - 4
  mpz_urandomm(_random_bounded, state, _aux); // _random_bounded := [0, n - 4]
  mpz_add_ui(_random_bounded, _random_bounded, 2L); // _random_bounded := [2, n - 2]
  mpz_sub_ui(_aux, candidate, 1L); // _aux := n - 1
  mpz_powm_sec(_x, _random_bounded, _d, candidate);

  if ( mpz_cmp_ui(_x, 1L) == 0 || mpz_cmp(_x, _aux) == 0 )
  {
    _p_flag = true;
    goto _end_miller_rabin;
  }

  while(mpz_cmp(_d, _aux) != 0)
  {
    mpz_powm_ui(_x, _x, 2L, candidate); // x := (x*x) % n
    mpz_mul_2exp(_d, _d, 1); // _d *= 2

    if ( mpz_cmp_ui(_x, 1L) == 0)
    {
      _p_flag = false;
      goto _end_miller_rabin;
    }

    if (mpz_cmp(_x, _aux) == 0)
    {
      _p_flag = true;
      goto _end_miller_rabin;
    }   
  }

  _end_miller_rabin:
    gmp_randclear(state);
    mpz_clears(_d, _x, _random_bounded, _aux, NULL);
    return _p_flag;
}