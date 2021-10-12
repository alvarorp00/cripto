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

char errbuff[ERRBUFF_LEN + 1];
bool cipher_status;

ssize_t max = MB1, // maximum length
          len = 0; // current offset

/* - - - - - - - - !! STATIC !! - - - - - - - - */

static char *load_from_file(FILE *i_file);

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

  mpz_t mz, az, bz;
  mpz_t gcd;
  mpz_t xz, cx, dx;
  
  char *input = NULL,
       *output = NULL;

  alphabet_t *alphabet;

  size_t i;
  
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
  
  // input --> plain text

  input = load_from_file(i_file);

  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't read from input file...");
    cipher_status = false;
    goto end_affine_cipher;
  }

  output = (char*)calloc(len + 1, sizeof(char)); // len(cipher_text) == len(plain_text)
  
  // fprintf(o_file, "%s\n", input);

  alphabet = alphabet_init(mpz_get_ui(mz));

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't start alphabet...");
    cipher_status = false;
    goto end_affine_cipher;
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't load alphabet from file...");
    cipher_status = false;
    goto end_affine_cipher;
  }

  mpz_inits(xz, cx, dx, NULL);

  for (i = 0; i < len; i++)
  {
    mpz_set_si(xz, alphabet_get_fromChar(alphabet, input[i]));

    if (opt == CIPHER)
    {
      mpz_mul(cx, az, xz);
      mpz_add(cx, cx, bz);
      mpz_mod(cx, cx, mz);
    }
    else // TODO!
    {
      // mpz_set_ui(cx, 1L);
      // mpz_invert(cx, az, cx);

      // if (mpz_sgn(cx) == 0)
      // {
      //   #line __LINE__ __FILE__
      //   gmp_snprintf(errbuff, ERRBUFF_LEN, "%Zd doesn't have multiplicative inverse... Stopping!", az);
      //   goto end_affine_cipher;
      // }

      // mpz_set_ui(dx, alphabet_get_fromChar(alphabet, input[i]));
      // mpz_sub(dx, dx, bz);
      // mpz_mul(cx, cx, dx);
      // mpz_mod(cx, cx, mz);
    }

    output[i] = alphabet_get_fromNum(alphabet, mpz_get_ui(cx)); // c -> char    
  }

  fflush(stdout);

  output[len] = '\0';

  fprintf(o_file, "%s\n", output);

  mpz_clears(xz, cx, dx, NULL);

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

static char *load_from_file(FILE *i_file)
{
  char *input;
  char c;
  
  if (!i_file)
    return NULL;
  
  input = (char*)calloc(MB1, sizeof(char));
  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    return NULL;
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

  return input;
}