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
#include <errno.h>

#include "alphabet.h"

#define A_NODES(a) (a)->nodes
#define A_NODES_AT(a, i) (a)->nodes[i]
#define A_MAX_SIZE(a) (a)->a_max_size
#define A_CURR_SIZE(a) (a)->curr_size

#define cmp_proto (uint_fast8_t)(void*, void*)

typedef struct
{
  int_fast8_t n;
}num_t;

typedef struct
{
  char c;
}char_t;

typedef struct
{
  num_t num;
  char_t chr;
}alphabet_node;

struct _alphabet_t{
  alphabet_node *nodes;
  uint8_t a_max_size;
  uint8_t curr_size;
};

static void quickSort(void *info, cmp_proto cmp)

/* ! -- STATIC DECLARATIONS -- ! */

uint_fast64_t c_hashcode(const char_t *c);

uint_fast64_t n_hashcode(const num_t *n);

bool char_cmp(const char_t *c1, const char_t *c2);

bool num_cmp(const num_t *n1, const num_t *n2);

/* ! -- END -- ! */

alphabet_t *alphabet_init(size_t a_size)
{
  alphabet_t alphabet;

  alphabet = (alphabet_t*)malloc(sizeof(alphabet_t));

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    goto alphabet_init_error;
  }
  
  A_NODES(alphabet) = (alphabet_node*)calloc(a_size, sizeof(alphabet_node));
  if (!A_NODES(alphabet))
  {
    #line __LINE__ __FILE__
    goto alphabet_init_error;
  }

  A_CURR_SIZE(alphabet) = 0;
  A_MAX_SIZE(alphabet) = a_size;
  
  return alphabet;

  alphabet_init_error:
    if (alphabet)
    {
      if (A_NODES(alphabet))
        free(A_NODES(alphabet))
      free(alphabet);
    }
    return NULL;
}

bool alphabet_map(alphabet_t *alphabet, char c, int_fast8_t n)
{
  alphabet_node node;
  size_t i;
  
  if (!alphabet || !c || !n)
  {
    #line __LINE__ __FILE__
    return false;
  }

  if (A_CURR_SIZE(alphabet) == A_MAX_SIZE(alphabet))
  {
    #line __LINE__ __FILE__
    return false;
  }

  node.chr.c = c;
  node.num.n = n;

  A_NODES_AT(alphabet, A_CURR_SIZE(alphabet)++) = node;

  return true;
}

const char *alphabet_get_fromNum(alphabet_t *alphabet, int_fast8_t n)
{
  size_t i;

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  for (i = 0, i < A_CURR_SIZE(alphabet); i++)
  {
    if (A_NODES_AT(alphabet, i).num == n)
    {
      return A_NODES_AT(alphabet, i).chr;
    }
  }

  return NULL;
}

int_fast8_t alphabet_get_fromChar(alphabet_t *alphabet, const char c)
{
  size_t i;

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  for (i = 0, i < A_CURR_SIZE(alphabet); i++)
  {
    if (A_NODES_AT(alphabet, i).chr == c)
    {
      return A_NODES_AT(alphabet, i).num;
    }
  }

  return NULL;
}

void alphabet_clean(alphabet_t *alphabet)
{
  if (alphabet)
  {
    if (A_NODES(alphabet))
      free(A_NODES(alphabet));
    free (alphabet);
  }
}

bool alphabet_loadFromFile(alphabet_t *alphabet, const char *filename, const char *pattern)
{
  FILE *file;
  
  char *buffer;
  const uint8_t max = 64; // Enough if empty spaces appear, but just 3 characters are needed (c sep n)

  char sep[max];
  char chr;
  int_fast8_t num;
  
  if (!alphabet || !filename)
  {
    #line __LINE__ __FILE__
    return;
  }

  file = fopen(filename, "r");
  if (!file)
  {
    #line __LINE__ __FILE__
    goto file_load_error;
  }

  // buffer = (char*)calloc(max + 1, sizeof(char));
  // if (!buffer)
  // {
  //   #line __LINE__ __FILE__
  //   goto file_load_error;
  // }

  // while (fgets(buffer, max - 1, file))
  // {
    
  // }


  return true;

  file_load_error:
    if (file)
      fclose(file)
    if (buffer)
      free(buffer);
    return false;
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