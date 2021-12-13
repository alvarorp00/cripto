/**
 * @file util.c
 * @author your name (you@domain.com)
 * @brief Definitions of helper functions
 * used in des algorithm
 * @version 0.1
 * @date 2021-11-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <string.h>
// #include <sodium.h>

#include "util.h"

byte bitAt(byte from, byte pos)
{
  return (from >> pos) & 1;
}

void setBitAt(byte *to, byte val, byte pos)
{
  *(to) = (((*(to) | (1 << pos)) ^ (1 << pos))) | (val << pos);
}

void computeModulus(mpz_t a, mpz_t b, mpz_t res)
{
  mpz_t _reg, _res;

  mpz_inits(_reg, _res, NULL);

  if (mpz_cmp(a, b) < 0)
  {
    mpz_set(_res, b);
    goto _skip_loop;
  }

  mpz_set(_reg, b);
  mpz_set(_res, a);

  while(mpz_cmp(_res, b) >= 0)
  {
    while (mpz_cmp(_reg, _res) <= 0)
      mpz_mul_2exp(_reg, _reg, 3);

    while (mpz_cmp(_reg, _res) > 0 && mpz_cmp(_reg, b) > 0)
      mpz_div_2exp(_reg, _reg, 3);

    if (mpz_cmp(_reg, b) < 0)
      mpz_set(_reg, b);

    while(mpz_cmp(_res, _reg) >= 0)
      mpz_sub(_res, _res, _reg);
  }

  _skip_loop:
    mpz_set(res, _res);
    mpz_clears(_reg, _res, NULL);
}