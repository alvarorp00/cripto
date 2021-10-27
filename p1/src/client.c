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
#include <errno.h>

#include "common.h"
#include "cipher.h"
#include "argparse.h"

char errbuff[ERRBUFF_LEN + 1];
bool cipher_status;

int main(int argc, char const *argv[])
{
  Parser *p = NULL;

  p = argparse_init();
    
  if (p == NULL)
  {
    eprintf("Error at %s while calling argparse_init(1)", __func__);
    goto end_main;
  }

  #ifdef __AFFINE__

    printf(" ! @@@ AFFINE CIPHER @@@ ! \n");
  
    char *encrypt = "-C";
    char *decrypt = "-D";
    char *ct_size = "-m";
    char *mc = "-a";
    char *ct = "-b";
    char *ipf = "-i";
    char *opf = "-o";

    argparse_add_argument(p, STR(encrypt), EMPTY, encrypt, 0, NULL);
    argparse_add_argument(p, STR(decrypt), EMPTY, decrypt, 0, NULL);
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

    opt = argparse_is_present(p, STR(encrypt)) ? CIPHER :
      argparse_is_present(p, STR(decrypt)) ? DECIPHER : CIPHER;

    m = argparse_get_arg(p, STR(ct_size));
    a = argparse_get_arg(p, STR(mc));
    b = argparse_get_arg(p, STR(ct));

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

  // #define __AFFINE_MOD__
  #ifdef __AFFINE_MOD__

    printf(" ! @@@ AFFINE CIPHER MOD @@@ ! \n");
    
    char *encrypt = "-C";
    char *decrypt = "-D";
    char *ct_size = "-m";
    char *mc = "-a";
    char *ct = "-b";
    char *ipf = "-i";
    char *opf = "-o";

    argparse_add_argument(p, STR(encrypt), EMPTY, encrypt, 0, NULL);
    argparse_add_argument(p, STR(decrypt), EMPTY, decrypt, 0, NULL);
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
    char *m;

    FILE *i_file = NULL,
         *o_file = NULL;
    
    char *a = NULL,
         *b = NULL;
    size_t i;

    opt = argparse_is_present(p, STR(encrypt)) ? CIPHER :
      argparse_is_present(p, STR(decrypt)) ? DECIPHER : CIPHER;

    m = argparse_get_arg(p, STR(ct_size));
    a = argparse_get_arg(p, STR(mc));
    b = argparse_get_arg(p, STR(ct));

    if (strlen(a) != strlen(b))
    {
      #line __LINE__ __FILE__
      eprintf("Keystring for (a,b) is of different size!");
      goto end_main;
    }

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

    cipher_status = false;

    affine_modified(opt, m, a, b, i_file, o_file);

    if (i_file != stdin && i_file)
    {
      fclose(i_file);
    }

    if (o_file != stdout && o_file)
    {
      fclose(o_file);
    }

    end_affine_mod:
      goto end_main;

  #endif

  #ifdef __CRYPTANALYZE_AFF_MOD__

    printf(" ! @@@ AFFINE_MOD_CIPHER CRYPTANALYSIS @@@ ! \n");
  
    char *ct_size = "-m";
    char *ipf = "-i";
    char *opf = "-o";

    argparse_add_argument(p, STR(ct_size), SINGLE, ct_size, 1, NULL);
    argparse_add_argument(p, STR(ipf), SINGLE, ipf, 1, NULL);
    argparse_add_argument(p, STR(opf), SINGLE, opf, 1, NULL);

    if (!argparse_parse_args(p, argc, argv))
    {
      eprintf("Error at %s while calling argparse_parse_args(3)", __func__);
      goto end_main;
    }

    char *m;

    FILE *i_file = NULL,
         *o_file = NULL;

    m = argparse_get_arg(p, STR(ct_size));

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

    affine_mod_cryptanalyze(m, i_file, o_file);

    if (i_file != stdin && i_file)
    {
      fclose(i_file);
    }

    if (o_file != stdout && o_file)
    {
      fclose(o_file);
    }

    // cipher_status = true; // we don't want program to blame us bcs of this...

    goto end_main;

  #endif

  #ifdef __HILL__

  #endif

  #ifdef __VIGENERE__
    printf(" ! @@@ VIGENERE @@@ ! \n");
    
    char *encrypt = "-C";
    char *decrypt = "-D";
    char *ct_size = "-m";
    char *key = "-k";
    char *ipf = "-i";
    char *opf = "-o";

    argparse_add_argument(p, STR(encrypt), EMPTY, encrypt, 0, NULL);
    argparse_add_argument(p, STR(decrypt), EMPTY, decrypt, 0, NULL);
    argparse_add_argument(p, STR(ct_size), SINGLE, ct_size, 1, NULL);
    argparse_add_argument(p, STR(key), SINGLE, key, 1, NULL);
    argparse_add_argument(p, STR(ipf), SINGLE, ipf, 1, NULL);
    argparse_add_argument(p, STR(opf), SINGLE, opf, 1, NULL);

    if (!argparse_parse_args(p, argc, argv))
    {
      eprintf("Error at %s while calling argparse_parse_args(3)", __func__);
      goto end_main;
    }

    enum OPTION opt;
    char *m, *k;

    FILE *i_file = NULL,
         *o_file = NULL;
    
    char *keystring = NULL;
    size_t i;

    opt = argparse_is_present(p, STR(encrypt)) ? CIPHER :
      argparse_is_present(p, STR(decrypt)) ? DECIPHER : CIPHER;

    m = argparse_get_arg(p, STR(ct_size));
    k = argparse_get_arg(p, STR(key));

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

    cipher_status = false;

    vigenere(opt, m, k, i_file, o_file);

    if (i_file != stdin && i_file)
    {
      fclose(i_file);
    }

    if (o_file != stdout && o_file)
    {
      fclose(o_file);
    }

  #endif

  #ifdef __CRYPTANALYZE_VIGENERE__
    printf(" ! @@@ CRYPTANALYZE VIGENERE @@@ ! \n");
    
    char *ct_size = "-m";
    char *ngram = "-l";
    char *ipf = "-i";
    char *opf = "-o";

    argparse_add_argument(p, STR(ct_size), SINGLE, ct_size, 1, NULL);
    argparse_add_argument(p, STR(ngram), SINGLE, ngram, 1, NULL);
    argparse_add_argument(p, STR(ipf), SINGLE, ipf, 1, NULL);
    argparse_add_argument(p, STR(opf), SINGLE, opf, 1, NULL);

    if (!argparse_parse_args(p, argc, argv))
    {
      eprintf("Error at %s while calling argparse_parse_args(3)", __func__);
      goto end_main;
    }

    char *m, *n;

    FILE *i_file = NULL,
         *o_file = NULL;
    
    
    m = argparse_get_arg(p, STR(ct_size));
    n = argparse_get_arg(p, STR(ngram));

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

    cipher_status = false;

    cryptanalyze_vigenere(m, n, i_file, o_file);

    if (i_file != stdin && i_file)
    {
      fclose(i_file);
    }

    if (o_file != stdout && o_file)
    {
      fclose(o_file);
    }
  #endif

  #ifdef __KASISKI__
    printf(" ! @@@ KASISKI TEST @@@ ! \n");
    
    char *ct_size = "-m";
    char *ngram = "-l";
    char *ipf = "-i";
    char *opf = "-o";

    argparse_add_argument(p, STR(ct_size), SINGLE, ct_size, 1, NULL);
    argparse_add_argument(p, STR(ngram), SINGLE, ngram, 1, NULL);
    argparse_add_argument(p, STR(ipf), SINGLE, ipf, 1, NULL);
    argparse_add_argument(p, STR(opf), SINGLE, opf, 1, NULL);

    if (!argparse_parse_args(p, argc, argv))
    {
      eprintf("Error at %s while calling argparse_parse_args(3)", __func__);
      goto end_main;
    }

    char *m, *n;

    FILE *i_file = NULL,
         *o_file = NULL;
    
    
    m = argparse_get_arg(p, STR(ct_size));
    n = argparse_get_arg(p, STR(ngram));

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

    cipher_status = false;

    kasiski(m, n, i_file, o_file);

    if (i_file != stdin && i_file)
    {
      fclose(i_file);
    }

    if (o_file != stdout && o_file)
    {
      fclose(o_file);
    }
  #endif

  #ifdef __IC__
    printf(" ! @@@ IC TEST @@@ ! \n");
    
    char *ct_size = "-m";
    char *ngram = "-l";
    char *ipf = "-i";
    char *opf = "-o";

    argparse_add_argument(p, STR(ct_size), SINGLE, ct_size, 1, NULL);
    argparse_add_argument(p, STR(ngram), SINGLE, ngram, 1, NULL);
    argparse_add_argument(p, STR(ipf), SINGLE, ipf, 1, NULL);
    argparse_add_argument(p, STR(opf), SINGLE, opf, 1, NULL);

    if (!argparse_parse_args(p, argc, argv))
    {
      eprintf("Error at %s while calling argparse_parse_args(3)", __func__);
      goto end_main;
    }

    char *m, *n;

    FILE *i_file = NULL,
         *o_file = NULL;
    
    
    m = argparse_get_arg(p, STR(ct_size));
    n = argparse_get_arg(p, STR(ngram));

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

    cipher_status = false;

    IC(m, n, i_file, o_file);

    if (i_file != stdin && i_file)
    {
      fclose(i_file);
    }

    if (o_file != stdout && o_file)
    {
      fclose(o_file);
    }
  #endif

  #ifdef __STREAM__

  #endif
  
  end_main:
  
    if (!cipher_status)
    {
      eprintf("Cipher didn't worked as expected: %s", errbuff);
      #ifdef __DEBUG__
      #endif
    }
    argparse_clean(p);
  
  return (cipher_status) ? 0 : 1;
}
