/**
 * @file utils.c
 * @author Álvaro Rodríguez (alvarorp00@sigsuspend.net)
 * @brief calc methods for Cripto assesment
 * @version 0.1
 * @date 2021-09-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "calclib.h"

void original_euclides_gcd(const mpz_t z, const mpz_t a, mpz_t gcd)
{
  // int_fast64_t ab, cd, residue;

  mpz_t ab, cd, residue;

  mpz_inits(ab, cd, residue, NULL);

  // ab = a > z ? a : z;
  // cd = a > z ? z : a;

  if (mpz_cmp(a, z) > 0)
  {
    mpz_set(ab, a);
    mpz_set(cd, z);
  }
  else
  {
    mpz_set(ab, z);
    mpz_set(cd, a);
  }

  // residue = cd;

  mpz_set(residue, cd);

  while (mpz_sgn(residue) != 0) // Just checks 1 bit
  {
    while (mpz_cmp(ab, cd) >= 0)
    {
      // ab -= cd;
      mpz_sub(ab, ab, cd);
    }

    // residue = ab;
    // ab = cd;
    // cd = residue == 0 ? cd : residue;

    mpz_set(residue, ab);
    mpz_set(ab, cd);
    
    if (mpz_sgn(residue) != 0) // Checks 1 bit
    {
      mpz_set(cd, residue);
    }
  }

  // return cd;
  mpz_set(gcd, cd);

  mpz_clears(ab, cd, residue, NULL);
}

void traditional_euclides_gcd(const mpz_t z, const mpz_t a, mpz_t gcd)
{
  mpz_t r0, r1, c;

  mpz_inits(r0, r1, c, NULL);

  // r0 = (a > z) ? a : z; // r1
  // r1 = (a > z) ? z : a; // r0

  if (mpz_cmp(a, z) > 0)
  {
    mpz_set(r0, a);
    mpz_set(r1, z);
  }
  else
  {
    mpz_set(r0, z);
    mpz_set(r1, a);
  }

  while (mpz_sgn(r1) != 0)
  {
    // c = r1;
    mpz_set(c, r1);
    // r1 = (r0 % r1);
    mpz_mod(r1, r0, r1);
    // r0 = c;
    mpz_set(r0, c);
  }

  // return r0;
  mpz_set(gcd, r0);

  mpz_clears(r0, r1, c, NULL);
}

void extended_euclides_gcd(const mpz_t z, const mpz_t a, mpz_t gcd)
{
  // int_fast64_t r0, r1, s0, t0, s1, t1, c0, qi;

  mpz_t r0, r1, s0, t0, s1, t1, c0, qi, cx;

  mpz_inits(r0, r1, s0, t0, s1, t1, c0, qi, cx, NULL);

  // if (z == a)
  //   return a;
  
  // r0 = (a > z) ? a : z;
  // r1 = (a > z) ? z : a;

  if (mpz_cmp(a, z) > 0)
  {
    mpz_set(r0, a);
    mpz_set(r1, z);
  }
  else
  {
    mpz_set(r0, z);
    mpz_set(r1, a);
  }

  // s0 = 1; t0 = 0; s1 = 0; t1 = 1;

  mpz_set_ui(s0, 1); mpz_set_ui(t0, 0); mpz_set_ui(s1, 0); mpz_set_ui(t1, 1);

  while (mpz_sgn(r1) != 0) // just checking one bit
  {
  //   c0 = r1;
  //   qi = (r0 / r1);
  //   r1 = (r0 % r1);
  //   r0 = c0;

  mpz_set(c0, r1);
  mpz_div(qi, r0, r1);
  mpz_mod(r1, r0, r1);
  mpz_set(r0, c0);

  //   c0 = s1;
  //   s1 = s0 - qi * s1;
  //   s0 = c0;

  mpz_set(c0, s1);
  mpz_mul(cx, s1, qi);
  mpz_sub(s1, s0, cx);
  mpz_set(s0, c0);

  //   c0 = t1;
  //   t1 = t0 - qi * t1;
  //   t0 = c0;

  mpz_set(c0, t1);
  mpz_mul(cx, t1, qi);
  mpz_sub(t1, t0, cx);
  mpz_set(t0, c0);
  
  } // r_i-1 = a * s_i-1 + b * t_i-1

  // return r0;

  mpz_set(gcd, r0);

  mpz_clears(r0, r1, s0, t0, s1, t1, c0, qi, cx, NULL);
}