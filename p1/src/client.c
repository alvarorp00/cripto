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

#include "common.h"
#include "cipher.h"
#include "argparse.h"

int main(int argc, char const *argv[])
{
  Parser *p = NULL;

  printf("Starting...\n");

  #ifdef __AFFINE__
    char *ct_size = "-m";
    char *mc = "-a";
    char *ct = "-b";
    char *ipf = "-i";
    char *opf = "-o";
  
    p = argparse_init();
    
    if (p == NULL)
    {
      eprintf("Error at %s while calling argparse_init(1)", __func__);
      goto end_main;
    }

    // if (!argparse_add_argument(p, argname, MULTIPLE, argpattern, nargs, NULL))
    // {
    //   eprintf("Error at %s while calling argparse_add_argument(6)", __func__);
    //   goto end_main;
    // }

    argparse_add_argument(p, STR(ct_size), SINGLE, ct_size, 1, NULL);
    argparse_add_argument(p, STR(mc), SINGLE, mc, 1, NULL);
    argparse_add_argument(p, STR(ct), SINGLE, ct, 1, NULL);
    argparse_add_argument(p, STR(ipf), SINGLE, ipf, 1, NULL);
    argparse_add_argument(p, STR(opf), SINGLE, opf, 1, NULL);

    if (!argparse_parse_args(p, argc, argv))
    {
      eprintf("Error at %s while calling argparse_parse_args(3)", __func__);
      goto end_main;
    }

    // argparse_print_args(p, stdout);
    
  #endif

  #ifdef __HILL__

  #endif

  #ifdef __VIGENERE__

  #endif

  #ifdef __FLOW__

  #endif
  
  end_main:
    argparse_clean(p);
  
  return 0;
}
