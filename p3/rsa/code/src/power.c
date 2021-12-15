/**
 * @file calclib.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "power.h"
#include "util.h"

#define BASE(d) (d)->base
#define POWER(d)  (d)->pow
#define MOD(d)  (d)->modulus

typedef enum _addition_chain_action_t
{
  NOTHING,
  DOUBLE_ONLY, // double previous value
  ADD_ONE_ONLY // add one to the previous value
} addition_chain_action_t;

struct _chain_node
{
  uint_fast64_t power; // value of this node
  addition_chain_action_t action; // action applied over previous value in array
  mpz_t value;
};

typedef struct _addition_chain_t
{
  struct _chain_node **nodes;
  uint_fast64_t length;
  uint_fast64_t max;
} addition_chain_t;

struct _power_t {
  mpz_t base;
  mpz_t pow;
  mpz_t modulus;
};

/* LOCAL STATIC METHOD DEFINITIONS */

/**
 * @brief Calculates addition
 * chain for given exponent
 * 
 * @param exponent whose addition chain is going to
 * be computed
 * @param chain return values here
 * @return mpz_power_error_t possible error
 */
static mpz_power_error_t _addition_chain(mpz_t exponent, addition_chain_t *chain);

/**
 * @brief Builds new
 * node of the addition chain
 * 
 * @param power power of the previous node
 * @param action to be applied over previous chain value
 * @return struct _chain_node* node returned
 */
static struct _chain_node *_new_chain_node (uint_fast64_t power, addition_chain_action_t action);

/**
 * @brief Cleans data
 * stored INSIDE chain,
 * it does not free
 * CHAIN pointer as it can
 * be inside heap and not only
 * in RAM, so that will be managed by caller
 * not by this method itself 
 * 
 * @param chain whose data will be freed
 */
static void _addition_chain_clean(addition_chain_t *chain);

/* PUBLIC IMPLEMENTATIONS */

power_t* power_init()
{
  power_t *data;
  
  data = NULL;
  data = (power_t*)calloc(1, sizeof(power_t));

  if (!data) return NULL;

  mpz_inits(data->base, data->pow, data->modulus, NULL);

  return data;
}

mpz_power_error_t power_setup(power_t *data, const char *b, const char *p, const char *m)
{
  if (!data)
    return NOT_INITIALIZED;

  if (!b || !p || !m)
    return NO_ARGUMENT_GIVEN;

  mpz_set_str(BASE(data), b, 10L);
  mpz_set_str(POWER(data),  p, 10L);
  mpz_set_str(MOD(data),  m, 10L);

  return OP_OK;
}

mpz_power_error_t power_setup_mpz(power_t *data, mpz_t b, mpz_t p, mpz_t m)
{
  if (!data)
    return NOT_INITIALIZED;

  mpz_set(BASE(data), b);
  mpz_set(POWER(data), p);
  mpz_set(MOD(data), m);

  return OP_OK;
}

mpz_power_error_t power_free(power_t *data)
{
  if (!data) return NOT_INITIALIZED;
  mpz_clears(data->base, data->pow, data->modulus, NULL);
  free(data);

  return OP_OK;
}

mpz_power_error_t power_compute(power_t *data, mpz_t result)
{
  mpz_power_error_t err;
  addition_chain_t  chain;

  uint_fast64_t _chain_at;
  
  if (!data) return NOT_INITIALIZED;

  if (mpz_cmp_ui(data->pow, 0L) == 0)
  {
    mpz_set_ui(result, 1L);
    mpz_mod(result, result, data->modulus);
    goto power_compute_ok;
  }
  else if (mpz_cmp_ui(data->pow, 1L) == 0)
  {
    mpz_set(result, data->base);
    mpz_mod(result, result, data->modulus);
    goto power_compute_ok;
  }

  if ((err = _addition_chain(POWER(data), &chain)) /* != 0 */)
  {
    _addition_chain_clean(&(chain));
    return err;
  }

  // we have now the addition chain, so it's very easy to compute values now

  mpz_set(chain.nodes[0]->value, data->base); // base value is first power

  for (_chain_at = 1; _chain_at < chain.length; _chain_at++)
  {
    switch (chain.nodes[_chain_at]->action)
    {
    case DOUBLE_ONLY:
      mpz_mul(chain.nodes[_chain_at]->value, chain.nodes[_chain_at - 1]->value, chain.nodes[_chain_at - 1]->value);
      break;
    case ADD_ONE_ONLY:
      mpz_mul(chain.nodes[_chain_at]->value, chain.nodes[_chain_at - 1]->value, data->base);
    default:
      break; // NOTHING
    }
    computeModulus(chain.nodes[chain.length - 1]->value, data->modulus, chain.nodes[chain.length - 1]->value);
  }

  mpz_set(result, chain.nodes[chain.length - 1]->value); // copy result

  // #define __VERBOSE
  #ifdef __VERBOSE
  for (_chain_at = 0; _chain_at < chain.length; _chain_at++)
    gmp_printf("# Partial value: %Zd\n\n", chain.nodes[_chain_at]->value);
  #endif

  _addition_chain_clean(&(chain));

  power_compute_ok:
    return OP_OK;
}

/* LOCAL STATIC METHODS */

static mpz_power_error_t _addition_chain(mpz_t exponent, addition_chain_t *chain)
{
  char     *strarray = NULL;
  byte_ptr binaryarray = NULL;
  char     buff[2]; // number + '\0'
  size_t   size;

  size_t  i;

  byte    _ld_1_fnd = 0; // flag to check if leading 1 has been toggled

  // addition_chain_action_t _action;
  uint_fast64_t           _prev_power;
  struct _chain_node      **_nodes;
  
  if (!chain) return NO_ARGUMENT_GIVEN;

  #define __CH_NODES_INIT_LENGTH 64

  chain->length = 0;
  chain->max    = __CH_NODES_INIT_LENGTH;
  _nodes = (struct _chain_node**)calloc(chain->max, sizeof(struct _chain_node*));

  if (!_nodes) return INIT_FAILURE;

  strarray = mpz_get_str(NULL, 2, exponent);
  if (!strarray)
    return INIT_FAILURE;
  size     = strlen(strarray);

  binaryarray = (byte_ptr)calloc(size + 2, sizeof(byte));
  if (!binaryarray)
    return INIT_FAILURE;

  buff[1] = '\0';

  for (i = 0; i < size; i++) // toggle leading 1b to 0b
  {
    buff[0] = strarray[i];
    binaryarray[i] = atoi(buff); // cast to int representation
    if (!_ld_1_fnd && binaryarray[i] == 1)
      {
        binaryarray[i] = 0;
        _nodes[(chain->length)++] = _new_chain_node(1, NOTHING);
        _ld_1_fnd = 1;
        continue; // we start now the process
      }
      else if (_ld_1_fnd != 0) // leading 1 has been found, no we process chain
      {
        _prev_power = _nodes[chain->length - 1]->power;
        if (binaryarray[i] == 1)
        {
          // _action = DOUBLE_ADD_ONE;
          _nodes[(chain->length)++] = _new_chain_node(_prev_power << 1, DOUBLE_ONLY);
          _prev_power = _nodes[chain->length - 1]->power;
          _nodes[(chain->length)++] = _new_chain_node(_prev_power + 1, ADD_ONE_ONLY);
        }
        else
        {
          // _action = DOUBLE_ONLY;
          _nodes[(chain->length)++] = _new_chain_node(_prev_power << 1, DOUBLE_ONLY);
        }
        if (chain->length == chain->max - 3) // at least 2 spaces...
        {
          _nodes = realloc(_nodes, (chain->max << 1));
          if (!_nodes)
            return INIT_FAILURE;
        }
      }
  } // first step done with O(log N) as it's a binary check
  chain->nodes = _nodes;

  #ifdef __VERBOSE
  for (i = 0; i < chain->length; i++ )
    printf("Power: %ld @ %d\n", chain->nodes[i]->power, chain->nodes[i]->action);
  #endif

  if (binaryarray != NULL)
    free(binaryarray);

  if (strarray != NULL)
    free(strarray);

  return OP_OK;
}

static struct _chain_node *_new_chain_node (uint_fast64_t power, addition_chain_action_t action)
{
  struct _chain_node *cnode = NULL;

  cnode = (struct _chain_node *)malloc(sizeof(struct _chain_node));
  if (!cnode) return NULL;

  cnode->power  = power;
  cnode->action = action;
  mpz_init(cnode->value);

  return cnode;
}

static void _addition_chain_clean(addition_chain_t *chain)
{
  uint_fast64_t _chain_at;
  
  if (chain)
  {
    for (_chain_at = 0; _chain_at < chain->length; _chain_at++)
    {
      if (chain->nodes[_chain_at])
      {
        mpz_clear(chain->nodes[_chain_at]->value);
        free(chain->nodes[_chain_at]);
      }
    }
    free(chain->nodes);
  }
}