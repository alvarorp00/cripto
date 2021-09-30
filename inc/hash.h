#ifndef _HASH_H
#define _HASH_H

#include <stdio.h>
#include <stdlib.h> /* for size_t */
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h> /* for fast integers */

#include "prototypes.h"

#define _DEF_HASHLEN_ 9
#define _HIGH_CRITICAL_FACTOR_ 0.6

/**
 * Hash type definition
 */
typedef struct _hash_t hash_t;

/**
 * Initialize hash structure
 * @param hashcode hashcode function
 * @param equals equals function
 * @param clean clean funciton
 * @return hash
 */
hash_t *hash_init(hashcode_t hashcode, equals_t equals, clean_t clean);

/**
 * Stores an element in hash
 * @param hash where element will be stored
 * @param info stored info
 * @return if it was posible
 */
bool hash_encode(hash_t *hash, void *info);

/**
 * Retrieves an element prev. stored in hash
 * @param hash where element is stored
 * @param info stored info
 * @return info retrieved
 */
void *hash_decode(hash_t *hash, void *info);

/**
 * Cleans memory
 * @param hash structure to delete
 */
void hash_clean(hash_t *hash);

/**
 * If info is stored
 * @param hash where info is stored
 * @param info info to check
 * @return if it's stored
 */
bool hash_contains(hash_t *hash, void *info);

#endif