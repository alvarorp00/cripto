#ifndef __CALCLIB_H__
#define __CALCLIB_H__

#include "gmp.h"
#include <stdint.h>

/**
 * DEBUG MODE -> prints execution process
 */

// #define __DEBUG__

#ifdef __DEBUG__
  #define TRACE eprintf("Err at: %s @%s : %s", __FILE__, __func__, __LINE__)
#else
  #define TRACE
#endif

#define eprintf(str, ...) \
          fprintf(stderr, ">> " str "\n", ##__VA_ARGS__)

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

int_fast64_t euclides_gcd(int_fast64_t z, int_fast64_t a);

int_fast64_t extendes_euclides_gcd(int_fast64_t z, int_fast64_t a);


#endif