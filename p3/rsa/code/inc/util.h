/**
 * @file util.h
 * @author your name (you@domain.com)
 * @brief File containing useful
 * methods used in des cipher
 * @version 0.1
 * @date 2021-11-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __DES_UTIL_H__
#define __DES_UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "gmp.h"

#ifdef __DEBUG__
  #define TRACE eprintf("Err at: %s @%s : %s", __FILE__, __func__, __LINE__)
#else
  #define TRACE fprintf(stderr, "");
#endif

#define eprintf(str, ...) \
          fprintf(stderr, ">> " str "\n", ##__VA_ARGS__)

typedef enum _def_err_code
{
  OP_OK = 0,
  NO_ARGUMENT_GIVEN,
  ILLEGAL_VALUE,
  ALLOC_EXCEPTION,
  ALREADY_INITIALIZED,
  NOT_INITIALIZED,
  INIT_FAILURE
} def_err_code;

/**
 * Converts literal to string
 * @param x literal to convert
 */
#define STR(x) #x

/**
 * Opens file in read mode
 * @param file to open
 */
#define READ(file) fopen(file, "r");

/**
 * Opens file in write mode
 * @param file to open
 */
#define WRITE(file) fopen(file, "w");

/**
 * Prints in stderr
 * @param format string to print
 * @param __VA_ARGS__ args to be formatted
 */
#define TO_STDERR(format, ...) \
            fprintf(stderr, ">>> " format ".\n", ##__VA_ARGS__)

/**
 * Prints in stdout
 * @param format string to print
 * @param __VA_ARGS__ args to be formatted
 */
#define TO_STDOUT(format, ...) \
            fprintf(stdout, ">>> " format ".\n", ##__VA_ARGS__)

/**
 * Prints in given file
 * @param file where to print
 * @param format string to print
 * @param __VA_ARGS__ args to be formatted
 */
#define TO_FILE(file, format, ...) \
            fprintf(file, format "\n", ##__VA_ARGS__)

/**
 * @brief Performs a circular
 * shift of x in l positions
 * to the left of a s-bit
 * length source
 * 
 * @param x to be shifted
 * @param l amount to be shifted to the left
 * @param s length of x in bits
 */
#define CLSHIFT(x,l,s) ((x<<l) | (x>> (s -l)))

/**
 * @brief Performs a circular
 * shift of x in r positions
 * to the right of a s-bit
 * length source
 * 
 * @param x to be shifted
 * @param r amount to be shifted to the right
 * @param s length of x in bits
 */
#define CRSHIFT(x,r,s) ((x>>r) | (x<< (s -r)))

typedef unsigned char byte;
typedef byte* byte_ptr;

#define KB1 1024
#define MB1 KB1 * KB1
#define GB1 MB1 * KB1 // too big!

/**
 * @brief Retrieves bit
 * at pos from given value
 * 
 * @param from to retrieve from
 * @param pos position of desired bit
 * @return byte 
 */
byte bitAt(byte from, byte pos);

/**
 * @brief Sets bit at position
 * with given value
 * 
 * @param to destination
 * @param val value of bit
 * @param pos position to update
 */
void setBitAt(byte *to, byte val, byte pos);

/**
 * @brief calculates modulus
 * of a and b, either with
 * a > b or b > a
 * 
 * @param a param
 * @param b param
 * @param res modulus 
 */
void computeModulus(mpz_t a, mpz_t b, mpz_t res);

#endif