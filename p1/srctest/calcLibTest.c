#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "calclib.h"

int main(int argc, char const *argv[])
{
  mpz_t a, z;
  mpz_t gcd;

  mpz_inits(a, z, NULL);

  if (argc < 3)
  {
    eprintf("Needs 2 arguments at least, 2 numbers whose gcd will be calculated\n");
    exit(EXIT_FAILURE);
  }

  mpz_set_str(a, argv[1], 10);
  mpz_set_str(z, argv[2], 10);

  // printf("GCD %ld & %ld :: %ld\n", z, a, extended_euclides_gcd(z, a));

  original_euclides_gcd(z, a, gcd);

  gmp_printf("GCD %Zd & %Zd :: %Zd\n", z, a, gcd);

  mpz_clears(a, z, gcd, NULL);
  
  return 0;
}
