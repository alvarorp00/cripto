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
//  #include "euclides.h"

#define KB1 1024
#define MB1 KB1 * KB1
#define GB1 MB1 * KB1 // too big!
#define BUFFER 256

#define ENG_IC 0.065
#define IC_THRESHOLD 0.005
#define MG_IDEAL_VALUE ENG_IC
#define MG_THRESHOLD IC_THRESHOLD

char errbuff[ERRBUFF_LEN + 1];
bool cipher_status;

#define LANGMODE ENGLISH // CASTILLIAN

struct Frequency{
  alphabet_t *alphabet; // must be provided
  size_t a_sz; // alphabet_size, must be provided
  char *textstring; // input text, needs to be set up first
  ssize_t textlen; // length of input text
  struct Param{
    char chr; // chr to search
    double prob; // probability of ocurrence in given text
    uint_fast64_t ocurrences; // number of times chr appears in given text
  } *chrs; // characters loaded by frequency structure from given alphabet
  struct IC{
    char **strs; // store final substrings
    size_t _M; // substrings length == len(Y_i) 
    size_t keylength; // length of the key
    float IC; // index of coincidence value
    bool ok; // return status for IC calc
  } IC;
  struct Kasiski{
    size_t keylength; // guessed key length
    size_t distance; // distance between repeated sequences
    char *str; // string match
    size_t *keycandidates; // keylength candidates
    size_t ncandidates; // number of keylength candidates
    bool ok; // status of kasiski performance
  } Kasiski;
  uint_fast32_t ngram; // ngram used for kasiski and IC test
};

/**
 * @brief Struct
 * used to compute
 * ic to determine
 * key total length from a given
 * input text and supposing
 * padding has been done before
 * encrypting source text
 * 
 */

/**
 * @brief 
 * 
 */
struct TextFrequencyIterator{
  alphabet_t *alphabet; // alphabet; must be supplied at initialization
  char *textstring; // texstring to check frequency
  struct TFNode {
    char chr;
    float prob;
    struct TFNode *next;
    struct TFNode *last;
  } *first; // first node in frequency iterator
  size_t sz; // number of nodes
  bool ok; // status of iterator build up
};

/* - - - - - - - - !! STATIC !! - - - - - - - - */

/**
 * @brief Loads data from given file
 * using alphabet. It performs a fseek
 * into de first position of the file so
 * it's not a destructive function as it
 * preserves given file pointer at initial position
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
static void _computeFrequency(struct Frequency *freq, char *textstring, ssize_t textlen);

/**
 * @brief Performs ic over given
 * input text
 * 
 * @param ic needs alphabet and input
 * values to be set before calling _IC
 * 
 * @return Data calculated is stored inside given structure 
 */
static void _IC(struct Frequency *freq, bool use_kasiski_candidates);

/**
 * @brief Cleans
 * data associated to
 * ic's structure
 * 
 * IMPORTANT: param is not alloc'd, JUST
 * PASSED BY REFERENCE NOT BY VALUE!
 * 
 * @param ic param to struct
 */
static void _freq_free(struct Frequency *freq);

/**
 * @brief Calculates index of coincidence from
 * given frequencies
 * 
 * @param freq structure used to calculate frequencies from alphabet
 * @return float IC
 */
static float _computePartialIC(struct Frequency *freq, size_t len);

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

static void _kasiski(struct Frequency *freq);
/**
 * @brief Initialize iterator
 * for given alphabet and textstring
 * (both supplied inside struct given as argument)
 * 
 * @param iterator
 */
static void _text_frequency_iterator_new(struct TextFrequencyIterator *iterator);

/**
 * @brief Returns text frequency node
 * at given posicion, starting at 0 (first => idx := 0)
 * 
 * @param iterator to check
 * @return struct TFNode* or NULL if it's out of bounds
 */
static struct TFNode* _text_frequency_iterator_at(struct TextFrequencyIterator *iterator, size_t idx);

/**
 * @brief Cleans data associated with given iterator, but
 * if argument was alloc'd dynamically, it won't free those
 * memory reference, just the ones that are inside given
 * structure.
 * 
 * So, if it's the case, call free(iterator) immediatly after calliing
 * this function
 * 
 * @param iterator 
 */
static void _text_frequency_iterator_clean(struct TextFrequencyIterator *iterator);

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

  alphabet_t *alphabet = NULL;
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
  
  if (mpz_cmp_ui(gcd, 1L) != 0)
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
    else
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

void affine_modified( enum OPTION opt, const char *m, char *a, char *b, FILE *i_file, FILE *o_file )
{  
  mpz_t mz, minv, *az, *bz; // keyspace is a vector!
  mpz_t gcd;
  mpz_t xz, cx, yz, dx;
  
  char *input = NULL,
       *output = NULL;

  alphabet_t *alphabet;
  int_fast8_t offset;

  ssize_t len = 0; // current offset
  size_t i, klength;

  if (!m || !a || !b)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    cipher_status = false;
    goto end_affine_mod_cipher;
  }

  if (strlen(a) != strlen(b))
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "Keystring (a,b) must be of same length");
    goto end_affine_mod_cipher;
  }

  klength = strlen(a); // equiv to strlen (b)

  mpz_inits(mz, gcd, minv, NULL);
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

  for (i = 0; i < klength; i++)
  {
    mpz_inits(az[i], bz[i], NULL);
    mpz_set_si(az[i], alphabet_get_fromChar(alphabet, a[i]));
    mpz_set_si(bz[i], alphabet_get_fromChar(alphabet, b[i])); 

    // THEOREM 2.1 The congruence ax ≡ b (mod m) has a unique solution x ∈ Zm for
    // every b ∈ Zm if and only if gcd(a, m) = 1.

    extended_euclides_gcd(az[i], mz, gcd); // gcd (a, m) = 1 --> in m=26, gcd(a, 26) = 1
    // euclides_ext(az[i], mz, gcd, minv); // gcd (a, m) = 1 --> in m=26, gcd(a, 26) = 1 <-- TODO

    if (mpz_cmp_ui(gcd, 1) != 0)
    {
      #line __LINE__ __FILE__
      gmp_printf("Failure EUCLIDES for gcd(%Zd, %Zd)=%Zd\n", az[i], mz, gcd);
      gmp_snprintf(errbuff, ERRBUFF_LEN, "gcd(%Zd, %Zd) = %Zd != 1", az[i], mz, gcd);
      cipher_status = false;
      goto end_affine_mod_cipher;
    }
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
    }
    else
    {
      mpz_set_si(yz, alphabet_get_fromChar(alphabet, input[i]));
      // mpz_set_ui(cx, 1L);
      mpz_invert(cx, az[(i%klength)], mz);
      // euclides_ext(az[(i%klength)], mz, gcd, minv); <-- TODO
      mpz_sub(dx, yz, bz[(i%klength)]);
      mpz_mul(cx, cx, dx);
      // mpz_mul(cx, minv, dx); <-- TODO
      mpz_mod(cx, cx, mz);
    }

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
    mpz_clears(mz, gcd, minv, NULL);
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
void affine_mod_cryptanalyze(const char *m, FILE *i_file, FILE *o_file)
{
  mpz_t mz, *az, *bz; // keyspace is a vector!
  mpz_t *az_exclude, *bz_exclude; // for next rounds...
  mpz_t gcd;
  mpz_t xz1, cx1, yz1, dx1;
  mpz_t xz2, cx2, yz2, dx2;
  
  char *input = NULL,
       *output = NULL;

  alphabet_t *alphabet;
  int_fast8_t offset;

  ssize_t len = 0,
          mk = 0; // current offset
  size_t i, j, k, l, n, c;

  struct TextFrequencyIterator tfiterator;
  struct Frequency freq;

  mpf_t M_g, px, fx;
  mpz_t ix;

  size_t kpos, idx, a_1, a_2, c_1, c_2, apsz, shftd;
  char *buffer;

  struct AlphabetIterator *apiterator;
  
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
  
  freq.alphabet = alphabet;
  freq.textstring = input;
  freq.textlen = strlen(input);

  _IC(&(freq), false);

  if (!freq.IC.ok)
    goto end_aff_mod_anlz;

  // printf("IC found: %f @ Key length: %ld\n", ic.IC, ic.keylength);
  // for (i = 0; i < ic._M; i++)
  // {
  //   printf("--> %s\n", ic.strs[i]);
  // }
  // printf("======================\n");

  /**
   * f_0 / ic._M , ... , f_25 / ic._M
   * f_ki / ic._M , ... , f_25+ki / ic._M
   * 
   * Notice: ic._M == n' => characters encrypted by each parameter of the key...
   * 
   * But as we're not in a shift-based cipher but in a affine modified one,
   * we have:
   * 
   * f0 / ic._M , ... , f25 / ic._M
   * 
   * | | | | | | | | | | | | | | | | |
   * v v v v v v v v v v v v v v v v v
   * 
   * (x + k_i) % 26 => (x*a + b) % 26
   * 
   * f_b / ic._M , ... , f_25*a+b / ic._M
   * 
   * -> At this moment we must remember that the ideal probability distribution
   * p0 , ... , p25 subscripts above formula is evaluated modulo 26
   * 
   * So p_i means distribution probability in frequence table given in
   * alphabet.
   * 
   * So instead of:
   * 
   * M_g <= Sum(0..25) := (p_i * f_(i + g)) / (n')
   * 
   * We'll be replacing:
   * 
   * f_(i + g) => f_(a*i + b)
   * 
   * And the pair (a,b) that gets closer
   * to de IC ideal value (0.065 used in ic)
   * will be the one selected for that row of
   * elements
   * 
   * Once done, we'll check if (a,b) is possible in Z_26 by
   * calculating it's gcd(a, mz) and if it's not possible we'll skip
   * those values and continue with next one closer to 0.065.
   * 
   * Another option would've been trying all (a,b) combinations
   * directly but won't use statistic information
   * as the other method.
   * 
   * For guessing (a, b), we'll use concepts of the
   * cryptanalysis of the normal affine cipher:
   * 
   * -> match main occurrence in cipher text
   *    with main occurrence in language
   * -> match second occurrence in cipher text
   *    with second main occurrence in language
   * -> if not, match third ocurrence in cipher text
   *    with second main ocurrencence in language
   * -> and so on...
   * 
   * So, both combined, we'll try to search an (a,b) pair
   * (both a and b in [0, 25]) and then compute it's M_g
   * value. For all (a, b) available (this means that 
   * gcd (a, mz) == 1), we'll get only the one closer
   * to ideal IC value, which is 0.065 as we've stated
   * above.
   */

  apiterator = alphabet_sortByFreq(alphabet, LANGMODE);
  if (!apiterator || !apiterator->ok)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_aff_mod_anlz;
    
  }
  
  tfiterator.alphabet = alphabet;

  az = (mpz_t*)calloc(freq.IC.keylength, sizeof(mpz_t));
  bz = (mpz_t*)calloc(freq.IC.keylength, sizeof(mpz_t));

  if (!az || !bz)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_aff_mod_anlz;
  }

  mpz_inits(mz, gcd, ix, NULL);
  mpf_inits(M_g, px, fx, NULL);
  
  // two bcs we're on a linear ecuation system!
  mpz_inits(xz1, cx1, yz1, dx1, NULL); // first pair of the congruence
  mpz_inits(xz2, cx2, yz2, dx2, NULL); // second pair of the congruence

  output = (char*)calloc((freq.IC.keylength * freq.IC._M) + 1, sizeof(char)); // +1 for trailing '\0'

  if (!output)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_aff_mod_anlz;
  }

  /**
   * We're indexing characters of Y_i in a top-down approach,
   * which means that for all strings matched, we'll
   * check it's characters in key applied order:
   * 
   *  This means that we're searching each part of the key
   *  in order: first the first (a,b), then the second...
   */

  buffer = (char*)calloc(freq.IC._M + 1, sizeof(char));
  if (!buffer)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_aff_mod_anlz;
  }

  apsz = alphabet_getCurrentSize(alphabet);
  mpz_set_str(mz, m, 10L);

  freq.alphabet = alphabet;
  freq.a_sz = alphabet_getCurrentSize(alphabet);

  for (kpos = 0; kpos < freq.IC.keylength; kpos++) // each part of the key...
  {
    mpz_inits(az[kpos], bz[kpos], NULL);

    mpz_set_ui(az[kpos], 0L);
    mpz_set_ui(bz[kpos], 0L);
    mpf_set_ui(M_g, 0L);

    buffer[0] = '\0'; // clear buffer
    
    strncpy(buffer, freq.IC.strs[kpos], freq.IC._M + 1);

    // printf("Buffer: %s\n", buffer);
    // continue;

    tfiterator.textstring = buffer;
    _text_frequency_iterator_new(&tfiterator);

    for (a_1 = 0; a_1 < apsz; a_1++)
    {
      mpz_set_ui(xz1, alphabet_get_fromChar(alphabet, alphabet_iteratorFreqAt(apiterator, a_1)->chr));
      for (c_1 = 0; c_1 < tfiterator.sz; c_1++)
      {
        mpz_set_ui(yz1, alphabet_get_fromChar(alphabet, _text_frequency_iterator_at(&tfiterator, c_1)->chr));
        for (a_2 = a_1 + 1; a_2 < apsz; a_2++)
        {
          mpz_set_ui(xz2, alphabet_get_fromChar(alphabet, alphabet_iteratorFreqAt(apiterator, a_2)->chr));
          mpz_sub(dx1, xz2, xz1);
          for (c_2 = c_1 + 1; c_2 < tfiterator.sz; c_2++)
          {
            mpz_set_ui(yz2, alphabet_get_fromChar(alphabet, _text_frequency_iterator_at(&tfiterator, c_2)->chr));
            mpz_sub(cx1, yz2, yz1);
            
            /**
             * We need to solve next congruence:
             * 
             * xz1 * az[kpos] + bz[kpos] = yz1
             * xz2 * az[kpos] + bz[kpos] = yz2
             * 
             * In general terms:
             * 
             * bz[kpos] = yz1 - (xz1 * az[kpos]) = yz2 - (xz2 * az[kpos])
             * 
             * So then:
             * 
             * (xz2 - xz1) * (az[kpos]) = (yz2 - yz1)
             * 
             * And finally:
             * 
             * az[kpos] = (yz2 - yz1) / (xz2 - xz1)
             * bz[kpos] = yz1 - xz1 * az[kpos]
             * 
             * And with the pair (az[kpos], bz[kpos]) => (a,b) we
             * could continue our algorithm...
             * 
             * - - - - - - - - - - - - - - - - - - - -
             * 
             * xz1 * az[kpos] + 1*bz[kpos] - yz1 = 0
             * xz2 * az[kpos] + 1*bz[kpos] - yz2 = 0
             * 
             * az[kpos] = ((1)*(-yz2) - (1)*(-yz1)) / ((xz1)*(1) - (xz2)*(1))
             * bz[kpos] = ((-yz1)*(xz2) - (-yz2)*(xz1)) / ((xz1)*(1) - (xz2)*(1))
             */
            
            mpz_div(az[kpos], cx1, dx1); // az[kpos] = (yz2 - yz1) / (xz2 - xz1)
            // mpz_invert(dx1, dx1, mz);
            // mpz_mul(az[kpos], cx1, dx1);

            extended_euclides_gcd(az[kpos], mz, gcd);

            if (mpz_cmp_ui(gcd, 1L) != 0)
              continue; //(a, b) pair is not valid!

            mpz_mul(cx1, xz1, az[kpos]);
            mpz_sub(bz[kpos], yz1, cx1); // bz[kpos] = yz1 - xz1 * az[pos]
            
            // We've found a valid (a, b) pair...

            mpf_set_ui(M_g, 0L);
            for (i = 0; i < alphabet_getCurrentSize(alphabet); i++)
            {
              mpf_set_d(px, alphabet_getNumProb(alphabet, i + alphabet_get_offset(alphabet), ENGLISH));
              
              mpz_set_ui(ix, i);
              mpz_mul(ix, az[kpos], ix);
              mpz_add(ix, ix, bz[kpos]);
              mpz_mod(ix, ix, mz);

              mpf_set_d(fx, _text_frequency_iterator_at(&tfiterator, mpz_get_ui(ix))->prob);
              mpf_mul_ui(fx, fx, 10L);
              // gmp_printf("\t --> (%Ff * %Ff) / (%ld)\n", px, fx, strlen(buffer));
              mpf_mul(px, px, fx);

              mpf_div_ui(px, px, strlen(buffer));
              mpf_add(M_g, M_g, px);
            }

            mpf_set_d(px, MG_IDEAL_VALUE);
            mpf_sub(fx, M_g, px);
            mpf_abs(fx, fx);

            // goto next_round;

            if (mpf_cmp_d(fx, MG_THRESHOLD) <= 0) // We've found it!
            {
              // gmp_printf("Valid pair: (%Zd, %Zd) \n", az[kpos], bz[kpos]);
              // gmp_printf("--> M_g [%ld / %ld] = %Ff\n", kpos + 1, freq.IC.keylength, M_g);
              // cmp if nexts az possible values are better
              goto next_round;
            }

            continue;
          }
        }
      }
    }
    next_round:
      _text_frequency_iterator_clean(&tfiterator);
  }

  // printf("--> ");
  // for (i = 0; i < freq.IC.keylength; i++)
  // {
  //   gmp_printf("(%Zd, %Zd) ", az[i], bz[i]);
  // }
  // printf("\n");

  if (buffer)
    free(buffer);

  // cipher_status = true;

  output = (char*)calloc((freq.IC._M * freq.IC.keylength) + 1, sizeof(char));
  if (!output)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
  }

  for (i = 0; i < strlen(input); i++)
  {
      mpz_set_si(yz1, alphabet_get_fromChar(alphabet, input[i]));
      // mpz_set_ui(cx, 1L);
      mpz_invert(cx1, az[(i%freq.IC.keylength)], mz);
      mpz_sub(dx1, yz1, bz[(i%freq.IC.keylength)]);
      mpz_mul(cx1, cx1, dx1);
      mpz_mod(cx1, cx1, mz);

    // output[i] = alphabet_get_fromNum(alphabet, mpz_get_ui(cx) + offset);
    output[i] = alphabet_get_fromNum(alphabet, mpz_get_ui(cx1)); // c -> char  
  }

  fprintf(o_file, "%s\n", output);
  fflush(o_file);

  cipher_status = true;

  end_aff_mod_anlz:
    _freq_free(&freq);
    alphabet_iteratorFree(apiterator);
    // _text_frequency_iterator_clean(&tfiterator); // already done above
    mpz_clears(mz, gcd, NULL);
    mpf_clears(M_g, fx, NULL);
    mpz_clears(xz1, cx1, yz1, dx1, NULL);
    mpz_clears(xz2, cx2, yz2, dx2, NULL);
    if (az)
    {
      for (i = 0; i < freq.IC.keylength; i++)
        mpz_clear(az[i]);
      free(az);
    }
    if (bz)
    {
      for (i = 0; i < freq.IC.keylength; i++)
        mpz_clear(bz[i]);
      free(bz);
    }
    if (output)
      free(output);
}

void vigenere(enum OPTION opt, const char *m, char *keystring, FILE *i_file, FILE *o_file)
{
  char *input = NULL,
       *output = NULL;
  
  alphabet_t *alphabet = NULL;

  mpz_t mz, gcd, cx;
  mpz_t *kz = NULL;

  ssize_t len = 0; // current offset
  size_t i, kl;
  
  if (!m || !keystring || !i_file || !o_file)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "bad arguments");
    goto end_vigenere;
  }
  
  if (!m || !keystring)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    cipher_status = false;
    goto end_vigenere;
  }

  mpz_inits(mz, gcd, cx, NULL);
  mpz_set_str(mz, m, 10L);
  kl = strlen(keystring);

  kz = (mpz_t*)calloc(kl, sizeof(mpz_t));
  if (!kz)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
    goto end_vigenere;
  }

  alphabet = alphabet_init(mpz_get_ui(mz));
  if (!alphabet)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't start alphabet...");
    cipher_status = false;
    goto end_vigenere;
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't load alphabet from file...");
    cipher_status = false;
    goto end_vigenere;
  }

  for ( i=0; i<kl; i++ )
  {
    mpz_init(kz[i]);
    mpz_set_si( kz[i], alphabet_get_fromChar(alphabet, keystring[i]) );
  }

  // input --> plain text

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "can't read from input file...");
    cipher_status = false;
    goto end_vigenere;
  }
  len = strlen(input);

  output = (char*)calloc(len + 1, sizeof(char)); // len(cipher_text) == len(plain_text)

  for ( i=0; i<len; i++ )
  {
    mpz_set_si( cx, alphabet_get_fromChar(alphabet, input[i]) );
    // gmp_printf("\t Cipher with %Zd value.\n", kz[(i%kl)]);

    if (opt == CIPHER)
      mpz_add(cx, cx, kz[( i % kl )]);
    else
      mpz_sub(cx, cx, kz[( i % kl )]);
    // gmp_printf("\t Shifted %c to %c\n", input[i], alphabet_get_fromNum(alphabet, mpz_get_si(cx)));
    mpz_mod(cx, cx, mz); // cx has either encrypted or decrypted character at i-th position of the text
    output[i] = alphabet_get_fromNum(alphabet, mpz_get_si(cx) );
  }

  output[len] = '\0'; // string length, trailing 0!

  // print output onto given stream...
  fprintf(o_file, "%s\n", output);
  cipher_status = true;

  end_vigenere:
    if (input)
      free(input);
    if (output)
      free(output);
    alphabet_clean(alphabet);
    if (kz)
    {
      for ( i=0; i<kl; i++ )
        mpz_clear(kz[i]);
      free(kz);
    }
    mpz_clears(mz, gcd, cx, NULL);
    return;
}

void cryptanalyze_vigenere(const char *m, const char *ngram, FILE *i_file, FILE *o_file)
{
  #define __KSK_AUTO "auto" // string to match if auto mode has been selected
  #define __KSK_AUTO_THRS 8 // threshold of auto search mode. Means kasiski will never be executed for ngrams bigger than 8 in auto mode
  
  mpz_t mz, *key; // mz -> alphabet size; *key => key vector
  mpf_t fx, gx, fig, pi, m_g, _m_g;
  
  char *input = NULL,
       *output = NULL;

  ssize_t max_ngram;
  
  alphabet_t *alphabet;
  struct Frequency freq = {0};
  struct Frequency substrfreq = {0};

  size_t i, j, g, idx;
  char *keystring = NULL;
  bool key_found;
  
  if (!m || !i_file || !o_file)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_cryptanalyze_vigenere;
  }

  mpz_init(mz);
  mpz_set_str(mz, m, 10L);

  alphabet = alphabet_init(mpz_get_ui(mz));
  if (!alphabet)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_cryptanalyze_vigenere;
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_cryptanalyze_vigenere;
  }

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_cryptanalyze_vigenere;
  }

  freq.alphabet = alphabet;
  freq.textstring = input;
  freq.textlen = strlen(input);

  if (!ngram || strcmp(ngram, __KSK_AUTO) == 0) // test maximum value for kasiski ngrams
  {
    max_ngram = 0;
    for ( i=2; i<freq.textlen; i++ )
    {
      freq.ngram = i;
      _kasiski(&(freq));
      if (freq.Kasiski.ok == false)
        break;
      max_ngram = i;
      if (max_ngram >= __KSK_AUTO_THRS)
        break;
    }
    freq.ngram = max_ngram; // we don't need to run kasiski again...
    freq.Kasiski.ok = true;
  }
  else
  {
    freq.ngram = (ngram != NULL) ? atol(ngram) : 0;
    _kasiski(&(freq));
  }

  if (freq.Kasiski.ok)
    _IC(&(freq), true);
  else
    _IC(&(freq), false);

  if (!freq.IC.ok)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "Coincidence index couldn't determine key length");
    goto end_cryptanalyze_vigenere;
  }

  // key length guessed at this point

  _computeFrequency(&(freq), freq.textstring, freq.textlen);

  // lets find closest G in M_g, 0 <= G <= 25 (in english) that
  // gives us a M_g closest to 0.065

  key = (mpz_t*)calloc(freq.IC.keylength, sizeof(mpz_t));
  if (!key)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_cryptanalyze_vigenere;
  }

  mpf_inits(m_g, _m_g, NULL);
  mpf_inits(fx, gx, pi, fig, NULL);
  
  substrfreq.alphabet = alphabet;

  for ( i=0; i<freq.IC.keylength; i++ ) // for each part of the key...
  {
    
    substrfreq.textstring = freq.IC.strs[i];
    substrfreq.textlen = strlen(substrfreq.textstring);
    substrfreq.chrs = (struct Param*)calloc(alphabet_getCurrentSize(alphabet), sizeof(struct Param));

    if (!substrfreq.chrs)
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
      goto end_cryptanalyze_vigenere;
    }

    _computeFrequency(&(substrfreq), substrfreq.textstring, substrfreq.textlen);

    /**
     * Frequencies analysis
     * 
     * 0 <= i < keylength
     * 
     * f_i is each node in text frequency iterator
     * n', which is string length, is strlen ( freq.IC.strs[i] ) 
     *     better than freq.IC._M as it can be unpadded and not
     *     be fit exactly in ( total_length / keylength )
     * strs[i] is each substring cipher by each part of the key,
     *         so strs[0] will be all characters supposedly encrypted
     *         by the first part of the key!
     */

    mpz_init(key[i]);
    key_found = false;

    mpf_set_ui(m_g, 0L);

    #ifdef __DEBUG__
    printf("Value of M_g(Y_%ld)\n", i+1);
    #endif
    
    for ( g=0; g<alphabet_getCurrentSize(alphabet); g++ )
    {
      // Lets see which value of g produces the closest
      // value of M_g to 0.065 defined as MG_IDEAL_VALUE
      mpf_set_ui(_m_g, 0L);
      for ( j=0; j<alphabet_getCurrentSize(alphabet); j++ )
      {
        // for each element of the alphabet...
        // -> Sum(0..25) <= (p_i * f_i+g) / (n') <- i = [0, 25]
        mpf_set_d(pi, alphabet_getNumProb(alphabet, j, ENGLISH)); // pi := p_i
        idx = ( (j + g) % alphabet_getCurrentSize(alphabet) ); // idx := (i + g) % 26 (for english alphabets)
        mpf_set_d(fig, substrfreq.chrs[ idx ].prob); // fig := ( f_i+g / n' ); as prob is not ocurrence but probability!
        mpf_mul(fx, pi, fig); // fx := p_i * f_i+g
        mpf_add(_m_g, _m_g, fx);
      }

      mpf_set_d(gx, MG_IDEAL_VALUE);

      mpf_sub(fx, m_g, gx);
      mpf_sub(gx, _m_g, gx);

      mpf_abs(fx, fx);
      mpf_abs(gx, gx);   

      #ifdef __DEBUG__
      gmp_printf("%Ff @ ", _m_g);
      #endif

      if ( mpf_cmp(gx, fx) < 0 ) // ¿ |gx| < |fx|? If yes, new M_g calculated is better and so G is our value
      {
        // gmp_printf("New M_G: %Ff @ Previous M_G: %Ff\n", _m_g, m_g);
        mpf_set(m_g, _m_g); // save new M_g calculated as it's better than previous one
        mpz_set_ui(key[i], g); // we've found part of the key!
        // printf("\n\t Key %ld: %ld\n", i+1, g);
        key_found = true;
      }
    }
    if (substrfreq.chrs)
    {
      free(substrfreq.chrs);
      substrfreq.chrs = NULL;
    }

    if (!key_found)
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "Part [%ld/%ld] of key couldn't be found!", i+1, freq.IC.keylength);
      break;
    }
    #ifdef __DEBUG__
    printf("\n\n");
    #endif
  }

  mpf_clears(m_g, _m_g, fx, gx, NULL);
  mpf_clears(pi, fig, NULL);

  if (!key_found)
    goto end_cryptanalyze_vigenere;
  
  keystring = (char*)calloc(freq.IC.keylength + 1, sizeof(char)); // +1 for trailing '\0'
  if (!keystring)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_cryptanalyze_vigenere;
  }

  for ( i=0; i<freq.IC.keylength; i++ )
  {
    keystring[i] = alphabet_get_fromNum(alphabet, mpz_get_si(key[i]) );
  }
  keystring[i] = '\0';

  vigenere(DECIPHER, m, keystring, i_file, o_file);

  #ifdef __DEBUG__
  printf("Key length: %ld\n", freq.IC.keylength);
  printf("KEYSTRING: %s\n", keystring);
  #endif

  end_cryptanalyze_vigenere:
    _freq_free(&freq);
    if (alphabet)
      alphabet_clean(alphabet);
    if (keystring)
      free(keystring);
    mpz_clear(mz);
    if (key)
    {
      for ( i=0; i<freq.IC.keylength; i++ )
        mpz_clear(key[i]);
      free(key);
    }
    return;
}

void kasiski(const char *m, const char *ngram, FILE *i_file, FILE *o_file)
{
  struct Frequency freq = {0};
  
  char *input = NULL;
  alphabet_t *alphabet = NULL;

  size_t i;
  
  if (!m || !i_file || !o_file)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_kasiski;
  }

  alphabet = alphabet_init(atoi(m));
  if (!alphabet)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_kasiski;
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_kasiski;
  }

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_kasiski;
  }

  freq.alphabet = alphabet;
  freq.textstring = input;
  freq.textlen = strlen(input);
  freq.ngram = atol(ngram);

  _kasiski(&(freq));

  if (!freq.Kasiski.ok)
  {
    snprintf(errbuff, ERRBUFF_LEN, "Kasiski couldn't find substr matching required n-gram");
    goto end_kasiski;
  }

  fprintf(o_file, "Kasiski Results [for n-gram of length %ld]: \n", freq.ngram);
  fprintf(o_file, "\t Distance: %ld\n", freq.Kasiski.distance );
  fprintf(o_file, "\t String matched: %s\n", freq.Kasiski.str );
  fprintf(o_file, "\t Candidates [total: %ld]:\n", freq.Kasiski.ncandidates);
  for ( i=0; i<freq.Kasiski.ncandidates; i++ )
    fprintf(o_file, "\t\t -> %ld\n", freq.Kasiski.keycandidates[i]);

  cipher_status = true;

  end_kasiski:
    _freq_free(&(freq));
    if (alphabet)
      alphabet_clean(alphabet);
    return; 
}

void IC(const char *m, const char *ngram, FILE *i_file, FILE *o_file)
{
  struct Frequency freq = {0};
  
  char *input = NULL;
  alphabet_t *alphabet = NULL;

  size_t i;
  
  if (!m || !i_file || !o_file)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_IC;
  }

  alphabet = alphabet_init(atoi(m));
  if (!alphabet)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_IC;
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_IC;
  }

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto end_IC;
  }

  freq.alphabet = alphabet;
  freq.textstring = input;
  freq.textlen = strlen(input);
  freq.ngram = (ngram != NULL) ? atol(ngram) : 0;

  if (freq.ngram != 0)
    _kasiski(&(freq));

  if (!freq.Kasiski.ok)
    _IC(&(freq), false);
  else
    _IC(&(freq), true);

  fprintf(o_file, "IC Results [for n-gram of length %ld]: \n", freq.ngram);
  fprintf(o_file, "\t IC found: %f\n", freq.IC.IC );
  fprintf(o_file, "\t Keylength guessed: %ld\n", freq.IC.keylength );
  fprintf(o_file, "\t Cipher Strings (Y_i):\n");
  for ( i=0; i<freq.IC.keylength; i++ )
    fprintf(o_file, "\t\t Y_%ld -> %s\n", i+1, freq.IC.strs[i]);

  cipher_status = true;

  end_IC:
    _freq_free(&(freq));
    if (alphabet)
      alphabet_clean(alphabet);
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
      continue; // so we do not accept input character if it's not recognised by alphabet given...
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

  fseek( i_file, 0, SEEK_SET ); // preserve i_file

  return input;
}

static void _computeFrequency(struct Frequency *freq, char *textstring, ssize_t textlen)
{
  size_t i;
  char c;
  
  if (!freq)
    return;
  
  freq->a_sz = alphabet_getCurrentSize(freq->alphabet);

  // printf("Textstring: %s\n", textstring);

  for (i = 0; i < freq->a_sz; i++)
  {
    freq->chrs[i].ocurrences = 0;
    freq->chrs[i].chr = alphabet_get_fromNum(freq->alphabet, i + alphabet_get_offset(freq->alphabet));
  }
  
  for (i = 0; i < textlen; i++)
  {
    c = textstring[i];
    if ((alphabet_contains_chr(freq->alphabet, c)))
    {
      (freq->chrs[alphabet_get_fromChar(freq->alphabet, c) 
        - alphabet_get_offset(freq->alphabet)].ocurrences)++;
    }
  }

  // We're doing another loop iteration
  // bcs we dont't want to perform
  // frequency prob calculation (floating point instruction...)
  // each time a character is read, so better
  // do it in another loop, which seems to be
  // quite fast as alphabets are not usually large
  
  for (i = 0; i < freq->a_sz; i++)
    freq->chrs[i].prob = (float)((float)(freq->chrs[i].ocurrences) / textlen);
}

static float _computePartialIC(struct Frequency *freq, size_t len)
{
  size_t i;
  float ic;

  if (!freq || len == 1)
    return 0;

  for (ic = 0.0, i = 0; i < freq->a_sz; i++)
  {
    // ic += pow(freq.chrs[i].prob, 2.0);
    ic += freq->chrs[i].ocurrences * (freq->chrs[i].ocurrences - 1);
  }
  ic /= (len * (len - 1));

  return ic;
}

static size_t *_get_divisors(ssize_t n, size_t *divs)
{
  size_t i, c, *k_divisors;
  size_t _divs;
  
  for (_divs = 0, i = 1; i <= (n >> 1); i++)
    if (!(n % i))
      _divs++;

  k_divisors = (size_t*)calloc(_divs, sizeof(size_t));
  if (!k_divisors)
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  for (c = 0, i = 2; i <= (n >> 1); i++)
    if (!(n % i))
    {
      k_divisors[c] = i;
      c++;
    }
  
  k_divisors[c] = n; // can be divided by itself too!
  *divs = _divs;
  
  return k_divisors;
}

static void _IC(struct Frequency *freq, bool use_kasiski_candidates)
{

  size_t m, i, j, k, c;
  size_t _M;

  char **substr;
  float IC;

  struct IC *ic;

  if (!freq)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
  }

  ic = &(freq->IC);
  ic->ok = false;

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

  freq->chrs = (struct Param*)calloc(alphabet_getCurrentSize(freq->alphabet), sizeof(struct Param));

  if (!freq->chrs)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
    goto _end_IC;
  }

  freq->a_sz = alphabet_getCurrentSize(freq->alphabet);
  ic->IC = 0;
  ic->_M = 0;
  ic->keylength = 0;

  // m stands for key length...

  for (i = 0; i < freq->textlen; i++)
  { 
    if (use_kasiski_candidates && i >= freq->Kasiski.ncandidates)
      break;
    m = (use_kasiski_candidates) ? freq->Kasiski.keycandidates[i] : ( i + 2 );
    substr = (char**)calloc(m, sizeof(char*));
    if (!substr)
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
      goto _end_IC;
    }

    IC = 0.0;
    for (j = 0; j < m; j++)
    {
      _M = (size_t)ceil((float)(freq->textlen) / (float)(m)); 
      substr[j] = (char*)calloc(_M + 1, sizeof(char)); // +1 for trailing '\0'
      if (!substr[j])
      {
        #line __LINE__ __FILE__
        snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
        goto _end_IC;
      }

      for (c = 0, k = j; k < freq->textlen; k += m, c++)
        substr[j][c] = freq->textstring[k];
      substr[j][c] = '\0';
      _computeFrequency(freq, substr[j], c);
      IC += _computePartialIC(freq, c);

      if (substr[j])
        free(substr[j]);
    }
    IC /= m;

    if (fabs(IC - ENG_IC) < fabs(ic->IC - ENG_IC))
    {
      ic->IC = IC;
      ic->_M = _M;
      ic->keylength = m;
      if (fabs(ic->IC - ENG_IC) <= IC_THRESHOLD)
        break;
    }

    if (substr)
      free(substr);
  }

  ic->strs = (char**)calloc(ic->keylength, sizeof(char*));
  if (!ic->strs)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto _end_IC;
  }

  for (j = 0; j < ic->keylength; j++)
  {
    ic->strs[j] = (char*)calloc(ic->_M + 1, sizeof(char)); // +1 for trailing '\0'
    if (!ic->strs[j])
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
      goto _end_IC;
    }
    for (c = 0, k = j; k < freq->textlen; k += ic->keylength, c++)
      ic->strs[j][c] = freq->textstring[k];
    ic->strs[j][c] = '\0';
  }
  ic->ok = true;

  _end_IC:
    return;
}

static void _kasiski(struct Frequency *freq)
{
  size_t i, j, k; // indexes

  char buffer[KB1 + 1];
  
  if (!freq)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "No frequency struct given");
    goto _end_kasiski;
  }

  freq->Kasiski.ok = false;

  if (freq->ngram == 0)
    goto _end_kasiski;

  /**
   * Does not check if keylength matched
   * divides whole text length as
   * in vigenere is possible to not fit
   * text length with key
   * 
   */

  // search for repeated sequences...
  for ( i=0; i<freq->textlen; i++ )
  {
    for ( j=i+1; j<freq->textlen; j++ )
    {
      for ( k=0; freq->textstring[i+k] == freq->textstring[j+k]; k++ )
        buffer[k] = freq->textstring[i+k]; // copy value
      buffer[k] = '\0'; // trailing 0!
      if ( k>=freq->ngram && (( j - i ) % k == 0) ) // greater or equal than value searched && distance divisible by ngram length
      {
        freq->Kasiski.ok = true;
        freq->Kasiski.distance = ( j - i ); // difference between two compared indexes
        freq->Kasiski.keycandidates = _get_divisors(freq->Kasiski.distance, &(freq->Kasiski.ncandidates));
        freq->Kasiski.str = (char*)calloc(k, sizeof(char));
        if (!freq->Kasiski.str)
          goto _end_kasiski;
        strncpy(freq->Kasiski.str, buffer, k);
        goto _end_kasiski;
      }
    }
  }

  _end_kasiski:
    return;
}

static void _freq_free(struct Frequency *freq)
{
  size_t i;
  
  if ( freq )
  {
    if (freq->chrs)
    {
      free(freq->chrs);
      freq->chrs = NULL;
    }
    if (freq->textstring)
    {
      free(freq->textstring);
      freq->textstring = NULL;
    }
    if (freq->IC.strs)
    {
      for (i = 0; i < freq->IC.keylength; i++)
      {
        if (freq->IC.strs[i])
          free(freq->IC.strs[i]);
      }
      free (freq->IC.strs);
    }
    if (freq->Kasiski.str)
      free(freq->Kasiski.str);
    if (freq->Kasiski.keycandidates)
      free(freq->Kasiski.keycandidates);
  }
  return;
}

// !!! TEXT FREQUENCY ITERATOR !!! ///

static void _text_frequency_iterator_new(struct TextFrequencyIterator *iterator)
{
  struct TFNode *new, *node, *_prev;
  struct Frequency freq;

  size_t i, j; char chr;

  iterator->first = NULL;
  iterator->ok = false;
  iterator->sz = 0;
  
  if (!iterator->alphabet || !iterator->textstring)
    return;

  freq.alphabet = iterator->alphabet;
  freq.a_sz = alphabet_getCurrentSize(iterator->alphabet);
  freq.chrs = (struct Param*)calloc(freq.a_sz, sizeof(struct Param));
  freq.textstring = iterator->textstring;
  freq.textlen = strlen(iterator->textstring);

  if (!freq.chrs)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto text_freq_new_end;
  }
  
  _computeFrequency(&freq, freq.textstring, freq.textlen);

  for (i = 0; i < freq.a_sz; i++)
  {
    new = (struct TFNode*)malloc(sizeof(struct TFNode));
    if (!new)
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
      goto text_freq_new_end;
    }
    new->chr = freq.chrs[i].chr;
    new->prob = freq.chrs[i].prob;
    new->last = NULL;
    new->next = NULL;

    if (iterator->first == NULL)
    {
      iterator->first = new;
      continue;
    }

    for (node = iterator->first;;node = node->next)
    {
      if (new->prob <= node->prob)
      {
        if (node->next == NULL)
        {
          node->next = new;
          new->last = node;
          (iterator->sz)++;
          break;
        }
        continue;
      }
      if (node->last != NULL)
      {
        node->last->next = new;
        new->last = node->last;
        node->last = new;
        new->next = node;
      }
      else
      {
        iterator->first = new;
        new->last = NULL;
        new->next = node;
        node->last = new;
      }
      (iterator->sz)++;
      break;
    }
  }

  iterator->ok = true;

  text_freq_new_end:
    if (freq.chrs) // don't call _freq_free as it's destructive and we would loose alphabet!
      free(freq.chrs);
}

static struct TFNode* _text_frequency_iterator_at(struct TextFrequencyIterator *iterator, size_t idx)
{
  struct TFNode *node;
  size_t i;
  
  for (i = 0, node = iterator->first; i < idx; i++, node = node->next)
  {
    if (!node)
      return NULL;
  }

  return node;
}

static void _text_frequency_iterator_clean(struct TextFrequencyIterator *iterator)
{
  struct TFNode *__inode, *__next_inode;

  __inode = iterator->first;

  if (!__inode)
    return;

  while(__inode->next != NULL)
  {
    __next_inode = __inode->next;
    free(__inode);
    __inode = __next_inode;
  }
  free(__inode);
}