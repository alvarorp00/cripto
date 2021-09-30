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

#include "calclib.h"

int_fast64_t euclides_gcd(int_fast64_t z, int_fast64_t a)
{
  int_fast64_t ab, cd, residue;

  ab = a > z ? a : z;
  cd = a > z ? z : a;

  residue = cd;

  while (residue != 0)
  {
    while (ab >= cd)
      ab -= cd;

    residue = ab;
    ab = cd;
    cd = residue == 0 ? cd : residue;
  }

  return cd;
}
