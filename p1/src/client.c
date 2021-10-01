/**
 * @file client.c
 * @author your name (you@domain.com)
 * @brief Client for ciphers
 * @version 0.1
 * @date 2021-09-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#define __AFFINE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cipher.h"
#include "argparse.h"

int main(int argc, char const *argv[])
{
  Parser *p = NULL;

  char argname[BUFSIZ + 1];
  char argpattern[BUFSIZ + 1];
  uint8_t *nargs = NULL;

  #ifdef __AFFINE__
    argparse_init(p);

    strcpy(argname, "input");
    strcpy(argpattern, "-i");
    *nargs = 1; 

    argparse_add_argument(
      p,
      argname,
      SINGLE,
      argpattern,
      nargs,
      NULL
    );

    argparse_parse_args(p, argc, (char**)argv);
  #endif

  #ifdef __HILL__

  #endif

  #ifdef __VIGENERE__

  #endif

  #ifdef __FLOW__

  #endif
  
  argparse_clean(p);
  
  return 0;
}
