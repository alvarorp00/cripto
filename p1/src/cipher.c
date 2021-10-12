/**
 * @file cipher.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-09-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "gmp.h"
#include "cipher.h"
#include "calclib.h"

#define KB1 1024
#define MB1 KB1 * KB1
#define GB1 MB1 * MB1 // too big!
#define BUFFER 256

/**
 * @brief Helper function for affine
 * 
 */
static void affine_cipher(
  const char *m,
  const char *a,
  const char *b,
  FILE *i_file,
  FILE *o_file
);

/**
 * @brief Helper function for affine
 * 
 */
static void affine_decipher(
  const char *m,
  const char *a,
  const char *b,
  FILE *i_file,
  FILE *o_file
);

char errbuff[ERRBUFF_LEN + 1];
bool cipher_status;

/* - - - - - - - - !! CODE !! - - - - - - - - */



/**
 * @brief Affine cipher using
 * numerical procedures of multiple precision
 * defined in GNU MP (GMP).
 * 
 * Verifies that both numbers given, a & b,
 * determine an affine inyective function.
 * If they not, it'll raise an error.
 * 
 * If input file is not given, standard input will be held until
 * data is provided.
 * 
 * If output file is not given, standard output will be used.
 * 
 */
void affine(
  enum OPTION opt,
  const char *m,
  const char *a,
  const char *b,
  FILE *i_file,
  FILE *o_file
)
{
  #ifdef __DEBUG__
    printf("Affine Configuration: \n");
    printf("--> Mode: %s\n", opt == CIPHER ? "cipher" : "decipher");
    printf("--> m: %s\n", m);
    printf("--> a: %s\n", a);
    printf("--> b: %s\n", b);
    // printf("--> i_file: %s\n", i_file);
    // printf("--> o_file: %s\n", o_file);
  #endif
  
  if (opt == CIPHER)
  {
    affine_cipher(m, a, b, i_file, o_file);
  }
  else
  {
    affine_decipher(m, a, b, i_file, o_file);
  }
}

static void affine_cipher(
  const char *m,
  const char *a,
  const char *b,
  FILE *i_file,
  FILE *o_file
)
{
  mpz_t mz, az, bz;
  mpz_t gcd;
  mpz_t xz, cx;
  
  char *input = NULL,
       *output = NULL;
  char c;

  alphabet_t *alphabet;

  size_t i;
  
  ssize_t max = MB1, // maximum length
          len = 0; // current offset
  
  if (!m || !a || !b)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    cipher_status = false;
    goto end_affine_cipher;
  }

  mpz_inits(mz, az, bz, gcd, NULL);

  mpz_set_str(mz, m, 10);
  mpz_set_str(az, a, 10);
  mpz_set_str(bz, b, 10);

  // THEOREM 2.1 The congruence ax ≡ b (mod m) has a unique solution x ∈ Zm for
  // every b ∈ Zm if and only if gcd(a, m) = 1.

  extended_euclides_gcd(az, mz, gcd); // gcd (a, m) = 1 --> in m=26, gcd(a, 26) = 1!
  
  if (mpz_cmp_ui(gcd, 1) != 0)
  {
    #line __LINE__ __FILE__
    gmp_snprintf(errbuff, ERRBUFF_LEN, "gcd(%Zd, %Zd) = %Zd != 1", az, mz, gcd);
    cipher_status = false;
    goto end_affine_cipher;
  }

  // Since b ∈ Zm, then we must assure this occurs:

  mpz_mod(bz, bz, mz); // So now b ∈ Zm

  input = (char*)calloc(MB1, sizeof(char));
  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    cipher_status = false;
    goto end_affine_cipher;
  }

  // read plain text

  if (i_file == stdin)
  {
    printf("-> Enter message (press CTRL + D in new line to finish): \n");
  }

  while((c = fgetc(i_file)) != EOF)
  {

    if(c == ' ' || c == '\t' || c == '\n')
      continue; // skip spaces and line jumps from cipher text!!

    input[len] = c;

    if (++len == max)
    {
      // expand input size
      input = realloc(input, (max <<= 1) * sizeof(char));
    }
  }

  input = realloc(input, (len + 1) * sizeof(char)); // truncate
  input[len] = '\0';
  
  // input --> plain text

  output = (char*)calloc(len + 1, sizeof(char)); // len(cipher_text) == len(plain_text)
  
  // fprintf(o_file, "%s\n", input);

  alphabet = alphabet_init(mpz_get_ui(mz));

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    goto end_affine_cipher;
  }

  alphabet_loadFromFile(alphabet, _DICT_FNAME);

  alphabet_print(alphabet, stdout);

  mpz_inits(xz, cx, NULL);

  for (i = 0; i < len; i++)
  {
    // mpz_set_si(xz, input[i]);

    // mpz_set_si(xz, alphabet_get_fromChar(input[i]));

    mpz_add(cx, xz, az);
    mpz_mod(cx, cx, mz);

    // c = alphabet_get_fromNum(mpz_get_si(cx)); // c -> char
    
  }

  fflush(stdout);

  output[len] = '\0';

  fprintf(o_file, "%s\n", output);

  mpz_clears(xz, cx, NULL);

  cipher_status = true;

  end_affine_cipher:
    mpz_clears(mz, az, bz, gcd, NULL);
    if (input != NULL)
      free(input);
    if (output != NULL)
      free(output);
    if (alphabet)
      alphabet_clean(alphabet);
}

static void affine_decipher(
  const char *m,
  const char *a,
  const char *b,
  FILE *i_file,
  FILE *o_file
)
{
  if (!m || !a || !b || !o_file)
  {
    #line __LINE__ __FILE__
    return;
  }
}