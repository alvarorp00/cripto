/**
 * @file prime.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __PRIME_H__
#define __PRIME_H__

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "util.h"
#include "gmp.h"

typedef enum _def_err_code prime_gen_err_t;

typedef struct _prime_gen_t prime_gen_t;

/**
 * @brief Generate structure used
 * 
 * @return prime_gen_t* 
 */
prime_gen_t *prime_generator_init();

/**
 * @brief Set up
 * structure with it's values
 * 
 * @param generator 
 * @param bits 
 * @param sec_level 
 * @param o_file 
 * @return prime_gen_err_t 
 */
prime_gen_err_t prime_generator_configure(prime_gen_t *generator, size_t bits, double err_level, FILE *o_file);

/**
 * @brief Generates
 * big prime number
 * and stores it inside res param
 * 
 * @param generator 
 * @param res where result is stored. Should be
 * previously initialized
 * @return prime_gen_err_t 
 */
prime_gen_err_t prime_generator_generate(prime_gen_t *generator, mpz_t res);

/**
 * @brief Cleans structure. Id does not
 * close file pointer given. Be careful.
 * 
 * @param generator 
 * @return prime_gen_err_t 
 */
prime_gen_err_t prime_generator_clean(prime_gen_t *generator);

#endif