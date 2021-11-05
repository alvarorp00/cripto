/**
 * @file des_main.c
 * @brief runs DES algorithm
 * @version 0.1
 * @date 2021-11-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "des.h"

int main(int argc, char const *argv[])
{
  des_t *des;
  des_error_t err;
  // size_t i;

  printf("DES Algorithm\n");

  des = des_new();

  if (!des)
  {
    printf("Failure initializing des\n");
    exit(EXIT_FAILURE);
  }

  byte def_key[7] = {0xFF};

  // printf("STARTER KEY: ");
  // for(i=0;i<7;i++)
  //   printf("%d ", def_key[i]);
  // printf("\n");

  err = des_configure(des, CFB, CIPHER, def_key, (byte_ptr)0, 16, 8, stdin, stdout, NULL);

  printf("ERR: %d\n", err);

  return 0;
}
