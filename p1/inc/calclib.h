#ifndef __CALCLIB_H__
#define __CALCLIB_H__

#include <stdint.h>

#include "common.h"
#include "gmp.h"

int_fast64_t euclides_gcd(int_fast64_t z, int_fast64_t a);

int_fast64_t extendes_euclides_gcd(int_fast64_t z, int_fast64_t a);


#endif