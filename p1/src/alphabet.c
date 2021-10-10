/**
 * @file alphabet.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-10-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdlib.h>

#include "alphabet.h"
#include "hash.h"

#define A_C_NUM(a) (a)->c_to_num
#define A_NUM_C(a) (a)->num_to_c
#define A_FILENAME(a) (a)->filename

typedef struct
{
  int_fast8_t n;
}num_t;

typedef struct
{
  char c;
}char_t;

struct _alphabet_t{
  hash *c_to_num;
  hash *num_to_c;
  const char *filename;
};

/* ! -- STATIC DECLARATIONS -- ! */

uint_fast64_t c_hashcode(const char_t *c);

uint_fast64_t n_hashcode(const num_t *n);

bool char_cmp(const char_t *c1, const char_t *c2);

bool num_cmp(const num_t *n1, const num_t *n2);

/* ! -- END -- ! */

alphabet_t *alphabet_init(const char *filename)
{
  alphabet_t alphabet;
  
  if (!filename)
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  alphabet = (alphabet_t*)malloc(sizeof(alphabet_t));

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    goto alphabet_init_error;
  }

  A_C_NUM(alphabet) = hash_init((hashcode_t)c_hashcode, (equals_t)char_cmp, free);
  A_NUM_C(alphabet) = hash_init((hashcode_t)n_hashcode, (equals_t)num_cmp, free);
  
  if (! A_C_NUM(alphabet) || ! A_NUM_C(alphabet))
  {
    #line __LINE__ __FILE__
    goto alphabet_init_error;
  }

  A_FILENAME(alphabet) = filename; // not dynamically alloc'd

  // load from file...
  
  return alphabet;

  alphabet_init_error:
    if (alphabet)
      free(alphabet);
    if (A_C_NUM(alphabet))
      hash_clean(A_C_NUM(alphabet))
    if (A_NUM_C(alphabet))
      hash_clean(A_NUM_C(alphabet))
    return NULL;
}

const char *alphabet_get_fromNum(alphabet_t *alphabet, int_fast8_t n)
{
  
}

int_fast8_t alphabet_get_fromChar(alphabet_t *alphabet, const char c)
{

}

void alphabet_clean(alphabet_t *alphabet)
{

}

/* ! -- STATIC IMPLEMENTATIONS -- ! */

uint_fast64_t c_hashcode(const char_t *c)
{
  if (!c)
    return UINT64_MAX;

  return (uint_fast64_t)(c->c);
}

uint_fast64_t n_hashcode(const num_t *n)
{
  if (!n)
    return UINT64_MAX;
  
  return (uint_fast64_t)(n->n);
}

bool char_cmp(const char_t *c1, const char_t *c2)
{
  if (!c1 || !c2)
    return false;
  return (c1->c == c2->c);
}

bool num_cmp(const num_t *n1, const num_t *n2)
{
  if (!n1 || !n2)
    return false;
  return (n1->n == n2->n);
}

