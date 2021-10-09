#ifndef __CALCLIB_H__
#define __CALCLIB_H__

#include <stdint.h>

#include "common.h"
#include "gmp.h"

void original_euclides_gcd(const mpz_t z, const mpz_t a, mpz_t gcd);

void traditional_euclides_gcd(const mpz_t z, const mpz_t a, mpz_t gcd);

void extended_euclides_gcd(const mpz_t z, const mpz_t a, mpz_t gcd);

#endif