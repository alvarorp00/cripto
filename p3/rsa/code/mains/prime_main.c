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
  LOG_INFO("Generacion de números primos: Miller-Rabin\n");

  prime_gen_err_t err;
  prime_gen_t     *generator = NULL;

  prime_params_t *params;

  size_t bits;
  double err_level;

  mpz_t res;

  generator = prime_generator_init();
  assert (generator != NULL);

  params = params_parse_prime(argc, argv);
  assert (params != NULL);

  bits = atol(params->bits);
  err_level  = atof(params->sec);

  err = prime_generator_configure(generator, bits, err_level, stdout);
  assert(err == OP_OK);

  err = prime_generator_generate(generator, res);
  assert(err == OP_OK);

  err = prime_generator_clean(generator);
  assert(err == OP_OK);
  
  return 0;
}
