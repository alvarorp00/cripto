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
#include "nxjson.h"

#define A_NODES(a) (a)->nodes
#define A_NODES_AT(a, i) (a)->nodes[i]
#define A_MAX_SIZE(a) (a)->a_max_size
#define A_CURR_SIZE(a) (a)->curr_size

typedef struct
{
  int_fast8_t num;
  char chr;
  double c_freq; //castillian frequency
  double e_freq; //english frequency
}alphabet_node;

struct _alphabet_t{
  alphabet_node *nodes;
  int_fast8_t a_max_size;
  int_fast8_t curr_size;
};

/* ! -- END -- ! */

alphabet_t *alphabet_init(size_t a_size)
{
  alphabet_t *alphabet;

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
        free(A_NODES(alphabet));
      free(alphabet);
    }
    return NULL;
}

bool alphabet_map(alphabet_t *alphabet, char c, int_fast8_t n)
{
  alphabet_node node;
  
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

  node.chr = c;
  node.num = n;

  A_NODES_AT(alphabet, A_CURR_SIZE(alphabet)++) = node;

  return true;
}

char alphabet_get_fromNum(alphabet_t *alphabet, int_fast8_t n)
{
  size_t i;

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    return 0;
  }

  for (i = 0; i < A_CURR_SIZE(alphabet); i++)
  {
    if (A_NODES_AT(alphabet, i).num == n)
    {
      return A_NODES_AT(alphabet, i).chr;
    }
  }

  return 0;
}

int_fast8_t alphabet_get_fromChar(alphabet_t *alphabet, const char c)
{
  size_t i;

  if (!alphabet)
  {
    #line __LINE__ __FILE__
    return -1;
  }

  for (i = 0; i < A_CURR_SIZE(alphabet); i++)
  {
    if (A_NODES_AT(alphabet, i).chr == c)
    {
      return A_NODES_AT(alphabet, i).num;
    }
  }

  return -1;
}

bool alphabet_loadFromFile(alphabet_t *alphabet, const char *filename)
{
  FILE *file;
  
  char *buffer; // using 1MB as max length
  int_fast64_t fsize;

  const nx_json *nxjson;
  const nx_json *arr, *item;

  size_t i;
  
  if (!alphabet || !filename)
  {
    #line __LINE__ __FILE__
    goto file_load_error;
  }

  file = fopen(filename, "r");
  if (!file)
  {
    #line __LINE__ __FILE__
    goto file_load_error;
  }

  fseek(file, 0L, SEEK_END);
  fsize = ftell(file);

  buffer = (char*)calloc(fsize + 1, sizeof(char)); // trailing '\0'
  if (!buffer)
  {
    #line __LINE__ __FILE__
    goto file_load_error;
  }
  
  rewind(file);
  fread(buffer, 1, fsize, file);
  buffer[fsize] = '\0';

  fclose(file); // all inside buffer!!

  nxjson = nx_json_parse(buffer, 0);

  if (!nxjson)
  {
    #line __LINE__ __FILE__
    goto file_load_error;
  }

  arr = nx_json_get(nxjson, "dictionary");

  if (!arr)
  {
    #line __LINE__ __FILE__
    goto file_load_error;
  }

  for (i = 0; i < arr->children.length; i++)
  {
    item = nx_json_item(arr, i);

    if (!item)
      break; // stop parsing...

    if (A_CURR_SIZE(alphabet) >= A_MAX_SIZE(alphabet))
      break; // stop parsing...

    A_NODES_AT(alphabet, A_CURR_SIZE(alphabet)).chr = nx_json_item(item, 0)->text_value[0];
    A_NODES_AT(alphabet, A_CURR_SIZE(alphabet)).num = nx_json_item(item, 1)->num.s_value;
    A_NODES_AT(alphabet, A_CURR_SIZE(alphabet)).c_freq = nx_json_item(item, 2)->num.dbl_value;
    A_NODES_AT(alphabet, A_CURR_SIZE(alphabet)).e_freq = nx_json_item(item, 3)->num.dbl_value;

    A_CURR_SIZE(alphabet)++;
  }

  if (buffer)
    free(buffer);
  if (nxjson)
    nx_json_free(nxjson);

  return true;

  file_load_error:
    if (file)
      fclose(file);
    if (buffer)
      free(buffer);
    if (nxjson)
      nx_json_free(nxjson);
    return false;
}

int_fast8_t alphabet_get_offset(alphabet_t *alphabet)
{
  return alphabet == NULL ? 0 : A_NODES_AT(alphabet, 0).num;
}

int_fast8_t alphabet_getCurrentSize(alphabet_t *alphabet)
{
  return alphabet == NULL ? 0 : A_CURR_SIZE(alphabet);
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

bool alphabet_contains_num(alphabet_t *alphabet, int_fast8_t num)
{
  size_t i;
  
  if (!alphabet)
    return false;
  
  for (i = 0; i < A_CURR_SIZE(alphabet); i++)
  {
    if (A_NODES_AT(alphabet, i).num == num)
      return true;
  }
  
  return false;
}

bool alphabet_contains_chr(alphabet_t *alphabet, char c)
{
  size_t i;
  
  if (!alphabet)
    return false;
  
  for (i = 0; i < A_CURR_SIZE(alphabet); i++)
  {
    if (A_NODES_AT(alphabet, i).chr == c)
      return true;
  }
  
  return false;
}

size_t alphabet_print(alphabet_t *alphabet, FILE *dest)
{
  size_t bytes, i;

  bytes = 0L;

  if (!alphabet || !dest)
    goto end_print;

  bytes += fprintf(dest, "[ \n");

  for (i = 0; i < A_CURR_SIZE(alphabet); i++)
  {
    bytes += fprintf(dest, "\t[%c | %d] $ C: %lf, E: %lf, \n",
      A_NODES_AT(alphabet, i).chr,
      A_NODES_AT(alphabet, i).num,
      A_NODES_AT(alphabet, i).c_freq,
      A_NODES_AT(alphabet, i).e_freq
    );
  }

  bytes += fprintf(dest, "]\n");

  end_print:
    return bytes;
}