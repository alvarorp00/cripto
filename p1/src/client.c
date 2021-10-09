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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "cipher.h"
#include "argparse.h"

char errbuff[ERRBUFF_LEN + 1];
bool cipher_status;

int main(int argc, char const *argv[])
{
  Parser *p = NULL;

  printf("Starting...\n");

  #ifdef __AFFINE__

    printf(" ! @@@ AFFINE CIPHER @@@ ! \n");
  
    char *encrypt = "-C";
    // char *decrypt = "-D";
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

    argparse_add_argument(p, STR(encrypt), EMPTY, encrypt, 0, NULL);
    // argparse_add_argument(p, STR(decrypt), EMPTY, decrypt, 0, NULL);
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

    enum OPTION opt;
    char *m, *a, *b;

    FILE *i_file = NULL,
         *o_file = NULL;

    opt = argparse_is_present(p, STR(encrypt)) ? CIPHER : DECIPHER;

    m = argparse_get_arg(p, STR(ct_size));
    a = argparse_get_arg(p, STR(mc));
    b = argparse_get_arg(p, STR(ct));

    // i_file = argparse_is_present(p, STR(ipf)) ? READ(argparse_get_arg(p, STR(ipf))) : stdin; 
    // o_file = argparse_is_present(p, STR(opf)) ? READ(argparse_get_arg(p, STR(opf))) : stdout;

    if (argparse_is_present(p, STR(ipf)))
    {
      // i_file = READ(argparse_get_arg(p, STR(ipf)));
      i_file = fopen(argparse_get_arg(p, STR(ipf)), "r");

      if (!i_file)
        eprintf("Could not open input file. Using stdin...");
    }
    i_file = (i_file == NULL) ? stdin : i_file;

    if (argparse_is_present(p, STR(opf)))
    {
      // o_file = WRITE(argparse_get_arg(p, STR(opf)));
      o_file = fopen(argparse_get_arg(p, STR(opf)), "w");

      if (!o_file)
        eprintf("Could not open output file. Using stdout...");
    }
    o_file = (o_file == NULL) ? stdout : o_file;

    affine(opt, m, a, b, i_file, o_file);

    if (!cipher_status)
    {
      eprintf("Cipher didn't worked as expected: %s", errbuff);
    }

    if (i_file != stdin && i_file)
    {
      fclose(i_file);
    }

    if (o_file != stdout && o_file)
    {
      fclose(o_file);
    }

    // argparse_print_args(p, stdout);

    goto end_main;
    
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
