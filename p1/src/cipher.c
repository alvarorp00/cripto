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
#define GB1 MB1 * KB1 // too big!
#define BUFFER 256

char errbuff[ERRBUFF_LEN + 1];
bool cipher_status;

struct Frequency{
  alphabet_t *alphabet;
  struct FrequencyParam *params;
  size_t a_sz;
};

struct FrequencyParam{
  char chr;
  double prob;
  uint_fast64_t ocurrences;
};

/* - - - - - - - - !! STATIC !! - - - - - - - - */

static char *load_from_file(FILE *i_file);

/**
 * @brief Calculates frequency
 * of each character inside alphabet
 * and stores info inside
 * 
 */
void computeFrequency(struct Frequency *freq, char *textstring, ssize_t len);

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
  mpz_t xz, cx, yz, dx;
  
  char *input = NULL,
       *output = NULL;

  alphabet_t *alphabet;
  int_fast8_t offset;

  ssize_t len = 0; // current offset
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

  // input --> plain text

  input = load_from_file(i_file);

  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't read from input file...");
    cipher_status = false;
    goto end_affine_cipher;
  }

  len = strlen(input);

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

  // alphabet_print(alphabet, stdout);

  mpz_inits(xz, cx, yz, dx, NULL);
  
  // offset = alphabet_get_offset(alphabet);

  for (i = 0; i < len; i++)
  {

    if (opt == CIPHER)
    {
      mpz_set_si(xz, alphabet_get_fromChar(alphabet, input[i]));
      mpz_mul(cx, az, xz);
      mpz_add(cx, cx, bz);
      mpz_mod(cx, cx, mz);
    }
    else // TODO!
    {
      mpz_set_si(yz, alphabet_get_fromChar(alphabet, input[i]));
      // mpz_set_ui(cx, 1L);
      mpz_invert(cx, az, mz);
      mpz_sub(dx, yz, bz);
      mpz_mul(cx, cx, dx);
      mpz_mod(cx, cx, mz);
    }

    // output[i] = alphabet_get_fromNum(alphabet, mpz_get_ui(cx) + offset);
    output[i] = alphabet_get_fromNum(alphabet, mpz_get_ui(cx)); // c -> char  
  }

  output[len] = '\0';

  fprintf(o_file, "%s\n", output);

  mpz_clears(xz, cx, yz, dx, NULL);

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

void affine_modified(
  enum OPTION opt,
  const char *m,
  char **a,
  char **b,
  uint8_t klength,
  FILE *i_file,
  FILE *o_file)
{
  mpz_t mz, *az, *bz; // keyspace is a vector!
  mpz_t gcd;
  mpz_t xz, cx, yz, dx;
  
  char *input = NULL,
       *output = NULL;

  alphabet_t *alphabet;
  int_fast8_t offset;

  ssize_t len = 0; // current offset
  size_t i;

  if (!m || !a || !b)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    cipher_status = false;
    goto end_affine_mod_cipher;
  }

  mpz_inits(mz, gcd, NULL);
  mpz_set_str(mz, m, 10);

  az = (mpz_t*)calloc(klength, sizeof(mpz_t));
  bz = (mpz_t*)calloc(klength, sizeof(mpz_t));

  if (!az || !bz)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    cipher_status = false;
    goto end_affine_mod_cipher;
  }

  for (i = 0; i < klength; i++)
  {
    mpz_inits(az[i], bz[i], NULL);
    mpz_set_str(az[i], a[i], 10);
    mpz_set_str(bz[i], b[i], 10); 

    // THEOREM 2.1 The congruence ax ≡ b (mod m) has a unique solution x ∈ Zm for
    // every b ∈ Zm if and only if gcd(a, m) = 1.

    extended_euclides_gcd(az[i], mz, gcd); // gcd (a, m) = 1 --> in m=26, gcd(a, 26) = 1!

    if (mpz_cmp_ui(gcd, 1) != 0)
    {
      #line __LINE__ __FILE__
      gmp_snprintf(errbuff, ERRBUFF_LEN, "gcd(%Zd, %Zd) = %Zd != 1", az, mz, gcd);
      cipher_status = false;
      goto end_affine_mod_cipher;
    }
  }
  
  // input --> plain text

  input = load_from_file(i_file);

  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't read from input file...");
    cipher_status = false;
    goto end_affine_mod_cipher;
  }

  len = strlen(input);

  output = (char*)calloc(len + 1, sizeof(char)); // len(cipher_text) == len(plain_text)
  
  // fprintf(o_file, "%s\n", input);

  alphabet = alphabet_init(mpz_get_ui(mz));

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't start alphabet...");
    cipher_status = false;
    goto end_affine_mod_cipher;
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't load alphabet from file...");
    cipher_status = false;
    goto end_affine_mod_cipher;
  }

  mpz_inits(xz, cx, yz, dx, NULL);

  for (i = 0; i < len; i++)
  {
    if (opt == CIPHER)
    {
      mpz_set_si(xz, alphabet_get_fromChar(alphabet, input[i]));
      mpz_mul(cx, az[(i%klength)], xz);
      mpz_add(cx, cx, bz[(i%klength)]);
      mpz_mod(cx, cx, mz);
      // gmp_printf("Val a: %Zd\t b: %Zd\n", az[(i%klength)], bz[(i%klength)]);
    }
    else
    {
      mpz_set_si(yz, alphabet_get_fromChar(alphabet, input[i]));
      // mpz_set_ui(cx, 1L);
      mpz_invert(cx, az[(i%klength)], mz);
      mpz_sub(dx, yz, bz[(i%klength)]);
      mpz_mul(cx, cx, dx);
      mpz_mod(cx, cx, mz);
    }

    // output[i] = alphabet_get_fromNum(alphabet, mpz_get_ui(cx) + offset);
    output[i] = alphabet_get_fromNum(alphabet, mpz_get_ui(cx)); // c -> char  
  }

  // alphabet_print(alphabet, stdout);

  fprintf(o_file, "%s\n", output);

  mpz_clears(xz, cx, yz, dx, NULL);

  cipher_status = true;

  end_affine_mod_cipher:
    for (i = 0; i < klength; i++)
    {
      mpz_clears(az[i], bz[i], NULL);
    }
    mpz_clears(mz, gcd, NULL);
    if (az)
      free(az);
    if (bz)
      free(bz);
    if (input)
      free(input);
    if (output)
      free(output);
    if (alphabet)
      alphabet_clean(alphabet);
    return;
}

// criptoanalyze vectorized affine cipher
// not very efficient at all...
void affine_mod_criptoanalyze(const char *m, FILE *i_file, FILE *o_file)
{
  mpz_t mz, *az, *bz; // keyspace is a vector!
  mpz_t gcd;
  mpz_t xz, cx, yz, dx;
  
  char *input = NULL,
       *output = NULL;

  alphabet_t *alphabet;
  int_fast8_t offset;

  ssize_t len = 0,
          mk = 0; // current offset
  size_t i, j, k, c;

  #define ENG_IC 0.065
  #define IC_THRESHOLD 0.02

  ssize_t *acceptable_mk;
  size_t *k_divisors;
  size_t divs, nsubstr;
  char **substr;

  struct Frequency freq;
  float IC;
  
  if (!m || !i_file || !o_file)
  {
    #line __LINE__ __FILE__
    goto end_aff_mod_anlz;
  }

  input = load_from_file(i_file);

  if (!input)
  {
    #line __LINE__ __FILE__
    goto end_aff_mod_anlz;
  }

  alphabet = alphabet_init(atoi(m));

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't start alphabet...");
    cipher_status = false;
    goto end_aff_mod_anlz;
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't load alphabet from file...");
    cipher_status = false;
    goto end_aff_mod_anlz;
  }

  len = strlen(input);

  for (c = 0, i = 1; i < len / 2; i++)
    if (!(len % i))
      c++;

  divs = c;
  k_divisors = (size_t*)calloc(divs, sizeof(size_t));
  if (!k_divisors)
  {
    #line __LINE__ __FILE__
    goto end_aff_mod_anlz;
  }

  for (c = 0, i = 1; i < len >> 1; i++)
    if (!(len % i))
        k_divisors[c++] = i;

  // now we have in k_divisors a set of all elements that divide len
  // although we should've performed previous computations in a more
  // efficient way, we're not taking care of that now

  // We're performing now following operation:
  //   y = y1y2...yn
  //   | | | | | | |
  //   | | | | | | |
  //   v v v v v v v
  //   y1 = y1 ym+1 y2m+1...
  //   y1 = y2 ym+2 y2m+2...
  //   y1 = y3 ym+3 y2m+3...
  //   ... ... ... ... ...
  //   y1 = ym y2m y3m...
  //   | | | | | | | |
  //   v v v v v v v v

  for (i = 0; i < divs; i++)
  {
    nsubstr = len / k_divisors[i];
    
    substr = (char**)calloc(nsubstr, sizeof(char*));
    if (!substr)
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
      goto end_aff_mod_anlz;
    }

    for (j = 0; j < nsubstr; j++)
    {
      substr[j] = (char*)calloc(k_divisors[i] + 1, sizeof(char)); // +1 for '\0' at the end...
      if (!substr[j])
      {
        #line __LINE__ __FILE__
        snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
        goto end_aff_mod_anlz;
      }
      for (c = 0, k = j; k < len; k += nsubstr, c++)
        substr[j][c] = input[k];
    }

    freq.alphabet = alphabet;
    freq.params = (struct FrequencyParam*)calloc(alphabet_getCurrentSize(alphabet), sizeof(struct FrequencyParam));

    if (!freq.params)
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
      goto end_aff_mod_anlz;
    }

    freq.a_sz = alphabet_getCurrentSize(alphabet);

    computeFrequency(&freq, input, len);
    IC = computeIC(freq);
  }
  // todo: calculate characters occurrence probability in given input text
  // so IC can then be calculated...

  end_aff_mod_anlz:
    if (input)
      free(input);
    if (output)
      free(output);
    if (alphabet)
      alphabet_clean(alphabet);
    if (substr)
    {
      for (i = 0; i < nsubstr; i++)
        if (substr[i])
          free(substr[i]);
      free(substr);
    }
    // exit properly
}

/* ! Static Helper Functions ! */

static char *load_from_file(FILE *i_file)
{
  ssize_t max = KB1, // maximum length
          len = 0; // current offset
  
  char *input;
  char c;
  
  if (!i_file)
    return NULL;
  
  input = (char*)calloc(KB1, sizeof(char));
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

    if(c == ' ' || c == '\t' || c == '\n') // can be fixed adding these to alphabet...
      continue; // skip spaces and line jumps from input text!!

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

void computeFrequency(struct Frequency *freq, char *textstring, ssize_t len)
{
  size_t i;
  char c;
  
  if (!freq || !textstring)
    return;

  for (i = 0; i < freq->a_sz; i++)
  {
    freq->params[i].ocurrences = 0;
    freq->params[i].chr = alphabet_get_fromNum(freq->alphabet, i + alphabet_get_offset(freq->alphabet));
  }
  
  for (i = 0; i < len; i++)
  {
    c = textstring[i];
    if ((alphabet_contains_chr(freq->alphabet, c)))
    {
      (freq->params[alphabet_get_fromChar(freq->alphabet, c) 
        - alphabet_get_offset(freq->alphabet)].ocurrences)++;
    }
  }

  // We're doing another loop iteration
  // bcs we dont't want to perform
  // frequency prob calculation (floating point instruction...)
  // each time a character is read, so better
  // do it in anoother loop, which seems to be
  // quite fast as alphabets are not usually large
  for (i = 0; i < freq->a_sz; i++)
  {
    freq->params[i].prob = freq->params[i].ocurrences / len;
  }
}

float computeIC(struct Frequency freq)
{
  #include <math.h>
  size_t i;
  float ic;

  for (ic = 0.0, i = 0; i < freq.a_sz; i++)
  {
    ic += pow(freq.params[i].prob, 2);
  }

  return ic;
}