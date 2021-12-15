/**
 * @file prime_main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <assert.h>

#include "prime.h"
#include "params.h"

/* ################################# */
/* ################################# */
/* ############# PROTO ############# */
/* ################################# */
/* ################################# */

static void _runExample(prime_gen_t *generator, prime_params_t *params, FILE *o_file);
static void _runTest(FILE *o_file);

/* ################################# */
/* ################################# */
/* ############# MAIN  ############# */
/* ################################# */
/* ################################# */

int main(int argc, char **argv)
{ 
  LOG_INFO("Generacion de números primos: Miller-Rabin\n");

  prime_gen_err_t err;
  prime_gen_t     *generator = NULL;

  prime_params_t *params;

  FILE *o_file;

  params = params_parse_prime(argc, argv);
  assert(params != NULL);

  o_file = params->def.outfPath == NULL ? stdout : fopen(params->def.outfPath, "w");

  if (params->def.doTest)
  {
    _runTest(o_file);
  }
  else
  {
    generator = prime_generator_init();
    assert(generator != NULL);

    err = prime_generator_configure(generator, atol(params->bits), atof(params->sec), o_file);
    assert(err == OP_OK);
    _runExample(generator, params, o_file);
    err = prime_generator_clean(generator);
    assert(err == OP_OK);
  }

  if (o_file && o_file != stdout)
    fclose(o_file);
  
  return 0;
}

/* ################################# */
/* ################################# */
/* ############# LOCAL ############# */
/* ################################# */
/* ################################# */

#define TRIES 1000

static void _runExample(prime_gen_t *generator, prime_params_t *params, FILE *o_file)
{
  struct prime_number_guess guess;
  prime_gen_err_t err;

  bool _min_test_success = false;

  assert(generator != NULL);
  assert(params != NULL);

  LOG_INFO("PERFORMING [SEARCH ONE PRIME @ bits: %s @ error margin: %s]\n", params->bits, params->sec);

  mpz_init(guess.candidate);

  if (params->min_test_suc)
  {
    #define MAX_TRIES 2048
    size_t i;

    LOG_INFO("\t SEARCHING UNTIL BEST HIT...\n");

    guess.prob_of_prime = 0.0f;
    for (i=0; i<MAX_TRIES; i++)
    {
      err = prime_generator_generate(generator, &guess);
      assert (err == OP_OK);

      if (guess.tests_passed < guess.tests_run)
        continue;
      else
        break;
    }

    if (i==MAX_TRIES)
    {
      LOG_WARN("None of the %d generated passed all tests\n", MAX_TRIES);
      goto _end_example;
    }

    gmp_printf("Prime number generated: %Zd\n", guess.candidate);
    printf("\t Probability of being prime: %lf\n", guess.prob_of_prime);
    printf("\t Tests passed: [%ld] out of [%ld]\n", guess.tests_passed, guess.tests_run);
  }
  else
  {
    err = prime_generator_generate(generator, &guess);
    assert (err == OP_OK);

    gmp_printf("Prime number generated: %Zd\n", guess.candidate);
    printf("\t Probability of being prime: %lf\n", guess.prob_of_prime);
    printf("\t Tests passed: [%ld] out of [%ld]\n", guess.tests_passed, guess.tests_run);
  }
  _end_example:
    mpz_clear(guess.candidate);
}

/* ################################# */
/* ################################# */
/* ############# TESTS ############# */
/* ################################# */
/* ################################# */

#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include "sodium.h"

#define PRINT_TEST_HEADER(f) \
  fprintf(f, "PRIME_NUMBER\tBITS\tSELF(prob)\tGMP([0:no][1:probably][2:yes])\tSELF(ms)\tGMP(ms)\tDIFFERENCE(ms)\n");

size_t _currentTimeInMiliseconds()
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
 
static void _runTest(FILE *o_file)
{
  #define BITS_STEP 32
  #define BITS_START 96
  #define BITS_TOP 16384 // very very big numbers tbh

  #define ERR_LEVEL_BIG 0.9
  #define ERR_LEVEL_1_DOWN 0.1
  #define ERR_LEVEL_2_DOWN 0.01
  #define ERR_LEVEL_3_DOWN 0.001
  #define ERR_LEVEL_4_DOWN 0.0001
  #define ERR_LEVEL_5_DOWN 0.00001
  #define ERR_LEVEL_6_DOWN 0.000001

  #define _DEF_DUMPER_EXIT stderr

  double _l1, _l2, _l3, _l4, _l5, _l6;
  
  mpz_t sres, gres; // self function && gmp function results
  
  prime_gen_t *generator;
  prime_gen_err_t err;

  size_t _bits;
  
  size_t starting_t;
  size_t ending_t;
  
  size_t self_elapsed_ms;
  size_t gmp_elapsed_ms;

  struct prime_number_guess guess;
  int    mpz_prob_ans;

  // setup
  generator = prime_generator_init();
  assert(generator != NULL);
  
  mpz_init(guess.candidate);

  #define __LOOP_TEST_POP(_level)\
    err = prime_generator_configure(generator, _bits, _level, _DEF_DUMPER_EXIT);\
    assert (err == OP_OK);\
    starting_t      = _currentTimeInMiliseconds();\
    err             = prime_generator_generate(generator, &guess);\
    ending_t        = _currentTimeInMiliseconds();\
    self_elapsed_ms = ending_t - starting_t;\
    assert(err == OP_OK);\
    starting_t      = _currentTimeInMiliseconds();\
    mpz_prob_ans    = mpz_probab_prime_p(guess.candidate, guess.tests_run);\
    ending_t        = _currentTimeInMiliseconds();\
    gmp_elapsed_ms  = ending_t - starting_t;\
    gmp_fprintf(o_file, "%Zd\t%ld\t%lf\t%d\t%ld\t%ld\t%ld\n",\
      guess.candidate, _bits, guess.prob_of_prime, self_elapsed_ms, gmp_elapsed_ms, self_elapsed_ms - gmp_elapsed_ms);\

  for (_bits = BITS_START; _bits < BITS_TOP; _bits += BITS_STEP)
  {
    PRINT_TEST_HEADER(o_file);
    for (_l1 = ERR_LEVEL_BIG; _l1 > 0; _l1 -= ERR_LEVEL_1_DOWN)
    {
      __LOOP_TEST_POP(_l1);
    }

    fprintf(o_file, "\n\n");
    PRINT_TEST_HEADER(o_file);
    for (_l2 = ERR_LEVEL_1_DOWN; _l2 > 0; _l2 -= ERR_LEVEL_2_DOWN)
    {
        __LOOP_TEST_POP(_l2);
    }

    fprintf(o_file, "\n\n");
    PRINT_TEST_HEADER(o_file);
    for (_l3 = ERR_LEVEL_2_DOWN; _l3 > 0; _l3 -= ERR_LEVEL_3_DOWN)
    {
      __LOOP_TEST_POP(_l3);
    }

    fprintf(o_file, "\n\n");
    PRINT_TEST_HEADER(o_file);
    for (_l4 = ERR_LEVEL_3_DOWN; _l4 > 0; _l4 -= ERR_LEVEL_4_DOWN)
    {
      __LOOP_TEST_POP(_l4);
    }

    fprintf(o_file, "\n\n");
    PRINT_TEST_HEADER(o_file);
    for (_l5 = ERR_LEVEL_4_DOWN; _l5 > 0; _l5 -= ERR_LEVEL_5_DOWN)
    {
      __LOOP_TEST_POP(_l5);
    }

    fprintf(o_file, "\n\n");
    PRINT_TEST_HEADER(o_file);
    for (_l6 = ERR_LEVEL_5_DOWN; _l6 > 0; _l6 -= ERR_LEVEL_6_DOWN)
    {
      __LOOP_TEST_POP(_l6);
    }
  }
  
  mpz_clear(guess.candidate);
}
