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
#include <math.h>

#include "gmp.h"
#include "cipher.h"
#include "calclib.h"

#define KB1 1024
#define MB1 KB1 * KB1
#define GB1 MB1 * KB1 // too big!
#define BUFFER 256

char errbuff[ERRBUFF_LEN + 1];
bool cipher_status;

// TODO struct ClassicCipher{...}

struct Frequency{
  alphabet_t *alphabet;
  struct Param{
    char chr;
    double prob;
    uint_fast64_t ocurrences;
  } *params;
  size_t a_sz;
};

/**
 * @brief Struct
 * used to compute
 * kasiski to determine
 * key total length from a given
 * input text and supposing
 * padding has been done before
 * encrypting source text
 * 
 */
struct Kasiski{
  char *input; // input text, needs to be set up first
  size_t *k_divisors; // array with available divsors
  ssize_t len; // length of input text
  size_t divs; // amount of len divisors (== length(k_divisors))
  size_t nsubstr; // number of substrings
  char **strs; // store final substrings
  size_t _M; // key length 
  float IC; // index of coincidence
  struct Frequency freq; // frequency structure param, needs to be set up first
  bool ok;
};

/* - - - - - - - - !! STATIC !! - - - - - - - - */

/**
 * @brief 
 * 
 * @param i_file 
 * @param alphabet 
 * @return char* 
 */
static char *_load_from_file(FILE *i_file, alphabet_t *alphabet);

/**
 * @brief Calculates frequency
 * of each character inside alphabet
 * and stores info inside
 * 
 */
static void _computeFrequency(struct Frequency *freq, char *textstring, ssize_t len);

/**
 * @brief Calculates index of coincidence from
 * given frequencies
 * 
 * @param freq structure used to calculate frequencies from alphabet
 * @return float IC
 */
static float _computeIC(struct Frequency freq);

/**
 * @brief Returns a dynamic array
 * containing divisors of number
 * n and stores in divs number
 * of total divisors
 * 
 * @param n number whose divisors want to be calculated
 * @param divs size_t pointer where amount of divisors will be stored
 * @return size_t* 
 */
static size_t *_get_divisors(ssize_t n, size_t *divs);

/**
 * @brief Performs kasiski over given
 * input text
 * 
 * @param ksk needs alphabet and input
 * values to be set before calling _kasiski
 * 
 * @return Data calculated is stored inside given structure 
 */
static void _kasiski(struct Kasiski *ksk);

/**
 * @brief Cleans
 * data associated to
 * kasiski's structure
 * 
 * IMPORTANT: param is not alloc'd, JUST
 * PASSED BY REFERENCE NOT BY VALUE!
 * 
 * @param kasiski param to struct
 */
static void _kasiski_free(struct Kasiski *kasiski);

/* ! IMPLEMENTATIONS ! */

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

  // input --> plain text

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't read from input file...");
    cipher_status = false;
    goto end_affine_cipher;
  }
  len = strlen(input);

  output = (char*)calloc(len + 1, sizeof(char)); // len(cipher_text) == len(plain_text)

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
  
  // input --> plain text

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't read from input file...");
    cipher_status = false;
    goto end_affine_mod_cipher;
  }
  len = strlen(input);

  output = (char*)calloc(len + 1, sizeof(char)); // len(cipher_text) == len(plain_text)

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
void affine_mod_cryptanalyze(const char *m, FILE *i_file, FILE *o_file)
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

  struct Kasiski ksk;
  
  if (!m || !i_file || !o_file)
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

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    #line __LINE__ __FILE__
    goto end_aff_mod_anlz;
  }
  
  ksk.freq.alphabet = alphabet;
  ksk.input = input;

  _kasiski(&ksk);

  if (!ksk.ok)
    goto end_aff_mod_anlz;
  
  // printf("Index of coincidence: %.4f\nM found: %ld\n", ksk.IC, ksk._M);
  // for (i = 0; i < ksk.nsubstr; i++)
  //   printf("--> %s\n", ksk.strs[i]);

  // guess (a,b) for each string...

  struct AlphabetIterator *iterator;

  iterator = alphabet_sortByFreq(alphabet, ENGLISH);
  if (!iterator || !iterator->ok)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_aff_mod_anlz;
  }

  // fix anonymous structure access...

  // printf("Value at 4: %c\n", alphabet_iteratorFreqAt(iterator, 3)->chr);

  // todo: calculate characters occurrence probability in given input text
  // so IC can then be calculated...

  end_aff_mod_anlz:
    _kasiski_free(&ksk);
    alphabet_iteratorFree(iterator);
    if (output)
      free(output);
}

void vigenere(enum OPTION opt, const char *m, char *k, FILE *i_file, FILE *o_file)
{
  char *input;
  
  if (!m || !k || !i_file || !o_file)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "bad arguments");
    goto end_vigenere;
  }

  // TODO

  end_vigenere:
    return;
}

/* ! Static Helper Functions ! */

static char *_load_from_file(FILE *i_file, alphabet_t *alphabet)
{
  ssize_t max = KB1, // initial maximum length
          len = 0; // current offset
  
  char *input;
  char c;
  
  if (!i_file || !alphabet)
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
    if (alphabet_contains_chr(alphabet, c) == false)
      continue;
    // if(c == ' ' || c == '\t' || c == '\n') // can be fixed adding these to alphabet...
    //   continue; // skip spaces and line jumps from input text!!


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

static void _computeFrequency(struct Frequency *freq, char *textstring, ssize_t len)
{
  size_t i;
  char c;
  
  if (!freq || !textstring)
    return;

  // printf("Textstring: %s\n", textstring);

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
    freq->params[i].prob = (float)((float)(freq->params[i].ocurrences) / len);
}

static float _computeIC(struct Frequency freq)
{
  size_t i;
  float ic;

  for (ic = 0.0, i = 0; i < freq.a_sz; i++)
    ic += pow(freq.params[i].prob, 2.0);

  return ic;
}

static size_t *_get_divisors(ssize_t n, size_t *divs)
{
  size_t i, c, *k_divisors;
  size_t _divs;
  
  for (_divs = 0, i = 1; i <= (n >> 1); i++)
    if (!(n % i))
      _divs++;
  _divs++; // can be divided by itself too!

  k_divisors = (size_t*)calloc(_divs, sizeof(size_t));
  if (!k_divisors)
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  for (c = 0, i = 1; i <= (n >> 1); i++)
    if (!(n % i))
    {
      k_divisors[c] = i;
      c++;
    }
  
  k_divisors[c] = n; // can be divided by itself too!
  *divs = _divs;
  
  return k_divisors;
}

static void _kasiski (struct Kasiski *ksk)
{
  #define ENG_IC 0.065
  // #define IC_THRESHOLD 0.02

  size_t i, j, k, c;
  size_t __M;

  char **substr;
  float IC;

  ksk->ok = false;
  
  ksk->len = strlen(ksk->input);
  ksk->k_divisors = _get_divisors(ksk->len, &(ksk->divs));

  if (!ksk->k_divisors)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't load k_divisors...");
    goto end_kasiski;
  }

  // printf("divs: %ld\n", divs);
  // for (i = 0; i < divs; i++)
  //   printf("-> %ld\n", k_divisors[i]);

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

  ksk->freq.params = (struct Param*)calloc(alphabet_getCurrentSize(ksk->freq.alphabet), sizeof(struct Param));

  if (!ksk->freq.params)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
    goto end_kasiski;
  }

  ksk->freq.a_sz = alphabet_getCurrentSize(ksk->freq.alphabet);
  ksk->IC = 0;
  ksk->_M = 0;

  for (i = 0; i < ksk->divs; i++)
  {
    ksk->nsubstr = ksk->len / ksk->k_divisors[i];
    
    substr = (char**)calloc(ksk->nsubstr, sizeof(char*));
    if (!substr)
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
      goto end_kasiski;
    }

    IC = 0.0;
    for (j = 0; j < ksk->nsubstr; j++)
    {
      substr[j] = (char*)calloc(ksk->k_divisors[i] + 1, sizeof(char)); // +1 for '\0' at the end...
      if (!substr[j])
      {
        #line __LINE__ __FILE__
        snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
        goto end_kasiski;
      }
      for (c = 0, k = j; k < ksk->len; k += ksk->nsubstr, c++)
        substr[j][c] = ksk->input[k];
      substr[j][c] = '\0';
      // printf("Substr: %s\n\n", substr[j]);
      _computeFrequency(&(ksk->freq), substr[j], c); // strlen(sbstr[j]) == c...
      IC += _computeIC(ksk->freq);

      if (substr[j])
        free(substr[j]);
    }
    // printf("IC: %f @@ nsubstr: %ld\n", IC, nsubstr);
    IC = (float)((float) IC / (float)ksk->nsubstr);
    // printf("IC : %.4f @ M: %ld\n", IC, k_divisors[i]);
    // printf("\tIC: %f\n", IC);
    if (fabs(IC - ENG_IC) < fabs(ksk->IC - ENG_IC))
    {
      ksk->IC = IC;
      ksk->_M = ksk->k_divisors[i];  // _M has key length (e.g. k=3 -> _M = k)
      __M = ksk->nsubstr;
    }

    if (substr)
      free(substr);
  }

  ksk->nsubstr = __M;
  ksk->strs = (char**)calloc(ksk->nsubstr, sizeof(char*));

  if (!ksk->strs)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_kasiski;
  }

  for (j = 0; j < ksk->nsubstr; j++)
  {
    ksk->strs[j] = (char*)calloc(ksk->_M + 1, sizeof(char)); // +1 for trailing '\0'
    if (!ksk->strs[j])
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
      goto end_kasiski;
    }
    for (c = 0, k = j; k < ksk->len; k += ksk->nsubstr, c++)
      ksk->strs[j][c] = ksk->input[k];
    ksk->strs[j][c] = 0;
  }

  ksk->ok = true;

  end_kasiski:
    return;
}

static void _kasiski_free(struct Kasiski *kasiski)
{
  size_t i;
  
  if (!kasiski)
    return;
  if (kasiski->freq.params)
    free(kasiski->freq.params);
  if (kasiski->freq.alphabet)
    alphabet_clean(kasiski->freq.alphabet);
  if (kasiski->input)
    free(kasiski->input);
  if (kasiski->k_divisors)
    free(kasiski->k_divisors);
  if (kasiski->strs)
  {
    for (i = 0; i < kasiski->nsubstr; i++)
      if (kasiski->strs[i])
        free(kasiski->strs[i]);
    free (kasiski->strs);
  }
}