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

int main(int argc, char **argv)
{
  #define TRIES 1000
  
  LOG_INFO("Generacion de números primos: Miller-Rabin\n");

  prime_gen_err_t err;
  prime_gen_t     *generator = NULL;

  prime_params_t *params;

  size_t bits;
  double err_level;

  struct prime_number_guess guess;
  size_t i;

  generator = prime_generator_init();
  assert (generator != NULL);

  params = params_parse_prime(argc, argv);
  assert (params != NULL);

  bits = atol(params->bits);
  err_level  = atof(params->sec);
  mpz_init(guess.candidate);

  err = prime_generator_configure(generator, bits, err_level, stdout);
  assert(err == OP_OK);

  for (i=0; i<TRIES; i++)
  {
    err = prime_generator_generate(generator, &guess);
    assert(err == OP_OK);

    if (guess.is_prime)
    {
      gmp_printf("Prime number [%ld] generated: %Zd\n", i, guess.candidate);
      printf("\t Probability of being prime: %lf\n", guess.prob_of_prime);
      break;
    }
  }

  err = prime_generator_clean(generator);
  assert(err == OP_OK);

  mpz_clear(guess.candidate);
  
  return 0;
}
