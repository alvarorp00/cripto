/**
 * @file power.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "power.h"
#include "logger.h"
#include "params.h"
#include "util.h"

/* PROTOTYPES */

static void runExample(power_t *data, power_params_t *params, FILE *o_file);
static void runTest   (FILE *o_file);

/* MAIN */

int main(int argc, char **argv)
{
  power_t        *data = NULL;
  power_params_t *params;
  
  mpz_power_error_t err;

  FILE *o_file;
  
  LOG_INFO("Potenciacion de numeros enteros\n");

  params = params_parse_power(argc, argv);
  assert(params != NULL);

  o_file = params->def.outfPath == NULL ? stdout : fopen(params->def.outfPath, "w");

  if (params->def.doTest)
  {
    runTest(o_file);
  }
  else
  {
    data   = power_init();
    assert(data != NULL);

    err    = power_setup(data, params->base, params->pow, params->modulus);
    assert(err == OP_OK);
    runExample(data, params, o_file);
    err = power_free(data);
    assert(err == OP_OK);
  }

  if (o_file && o_file != stdout)
    fclose(o_file);
  
  return 0;
}

/* LOCAL */

static void runExample(power_t *data, power_params_t *params, FILE *o_file)
{
  mpz_t result;
  mpz_power_error_t err;
  
  assert(data != NULL);

  mpz_init(result);

  err = power_compute(data, result);
  assert(err == OP_OK);

  gmp_fprintf(o_file, "Self function result: %Zd\n", result);

  mpz_t base, pow, mod;
  mpz_inits(base, pow, mod, NULL);

  mpz_set_str(base, params->base, 10L);
  mpz_set_str(pow, params->pow, 10L);
  mpz_set_str(mod, params->modulus, 10L);

  mpz_powm(result, base, pow, mod);

  gmp_fprintf(o_file, "GMP function result : %Zd\n", result);

  mpz_clears(base, pow, mod, NULL);

  mpz_clear(result);
}

/* TEST */

#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include "sodium.h"

#define PRINT_TEST_HEADER(f) \
  fprintf(f, "BASE\tEXPONENT\tMODULUS\tSELF(ms)\tGMP(ms)\tDIFFERENCE\n");

size_t currentTimeInMiliseconds()
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/** 
 * base starting with 96 bits until ~4000b, steps of 32b
 * exponent chosen randomly between 256 and 65565
 * modulus chosen randomly between 15 and 45
 */
 
static void runTest(FILE *o_file)
{
  // #define TESTS 10000
  
  #define BASE_STEP 32
  #define BASE_START 96
  #define BASE_TOP 10000

  #define MOD_BOT_BOUND 15
  #define MOD_TOP_BOUND 45
  
  mpz_t base, exponent, module;
  mpz_t sres, gres; // self function && gmp function results
  
  power_t *data;
  mpz_power_error_t err;

  uint_least16_t _bits;
  
  size_t starting_t;
  size_t ending_t;
  
  size_t self_elapsed_ms;
  size_t gmp_elapsed_ms;

  // setup
  data = power_init();
  assert(data != NULL);
  
  PRINT_TEST_HEADER(o_file);

  mpz_inits(base, exponent, module, NULL);
  mpz_inits(sres, gres, NULL);

  mpz_set_ui(base, (unsigned long)(randombytes_random())); // 32 random bits
  mpz_mul_2exp(base, base, 32);

  mpz_set_ui(base, (unsigned long)(randombytes_random())); // 32 random bits
  mpz_mul_2exp(base, base, 32);

  mpz_set_ui(base, (unsigned long)(randombytes_random())); // 32 random bits
  mpz_mul_2exp(base, base, 32);

  // now we have 96 bits number in mpz
  
  for(_bits = BASE_START; _bits < BASE_TOP; _bits += BASE_STEP)
  {
    mpz_set_ui(module, (unsigned long)(randombytes_uniform(MOD_BOT_BOUND) + (MOD_TOP_BOUND - MOD_BOT_BOUND))); // random module
    mpz_set_ui(exponent, (unsigned long)((randombytes_uniform(MOD_BOT_BOUND << 2) << 2) + (MOD_TOP_BOUND - MOD_BOT_BOUND)) << 2); // random exponent

    power_setup(data, mpz_get_str(NULL, 10, base), mpz_get_str(NULL, 10, exponent), mpz_get_str(NULL, 10, module));
    starting_t = currentTimeInMiliseconds();
    err = power_compute(data, sres);
    ending_t = currentTimeInMiliseconds();
    self_elapsed_ms = ending_t - starting_t;

    assert(err == 0); // assert is fine, do not take time because of time performance

    starting_t = currentTimeInMiliseconds();
    mpz_powm(gres, base, exponent, module);
    ending_t = currentTimeInMiliseconds();
    gmp_elapsed_ms = ending_t - starting_t;

    assert(mpz_cmp(sres, gres) == 0); // assert calcs are ok

    // print here
    gmp_fprintf(o_file, "%Zd\t%Zd\t%Zd\t%ld\t%ld\t%ld\n",
      base, exponent, module, self_elapsed_ms, gmp_elapsed_ms, abs(self_elapsed_ms - gmp_elapsed_ms));
    
    mpz_mul_2exp(base, base, BASE_STEP); // shift left 32 bits
    mpz_add_ui(base, base, (unsigned long)(randombytes_random())); // 32 random bits
  } 
  mpz_clears(base, exponent, module, NULL);
  mpz_clears(sres, gres, NULL);
}
