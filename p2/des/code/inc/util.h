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

#ifdef __DEBUG__
  #define TRACE eprintf("Err at: %s @%s : %s", __FILE__, __func__, __LINE__)
#else
  #define TRACE fprintf(stderr, "");
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
typedef uint32_t word;
typedef uint64_t dword;

/**
 * @brief Structure
 * to represent
 * 64-bit length number
 * in bytearray[8] either
 * 64b all-number 
 * 
 */
union bconv_t {
  byte bytes[8]; // bytearray
  dword l; // representation in 64b
};

#define KB1 1024
#define MB1 KB1 * KB1
#define GB1 MB1 * KB1 // too big!

/**
 * @brief Loads all data stored inside
 * given file until EOF.
 * 
 * @param i_file input file
 * @return data read
 */
char* load_all_from_file(FILE *i_file);

/**
 * @brief Prints msg into given file
 * 
 * @param o_file file
 * @param msg to print
 * @param size of msg
 * @return uint32_t amount of data printed
 */
uint32_t dump_to_file(FILE *o_file, char* msg, size_t size);

/**
 * @brief Converts char array
 * into base64
 * 
 * @param msg 
 * @return dword 
 */
dword string8ToB64(byte msg[]);

/**
 * @brief Generates
 * random 64b unsigned number
 * 
 */
dword get_random_key();

/**
 * @brief Reverses values at bit level,
 * performing lsb to msb and viceversa
 * 
 * @param n number to reverse
 * @param k bits to reverse (max: 64)
 * @return dword reversed number
 */
dword reverse(const dword n, const dword k);

/**
 * @brief Retrieves bit
 * at pos from given value
 * 
 * @param from to retrieve from
 * @param pos position of desired bit
 * @return byte 
 */
byte bitAt(dword from, byte pos);

/**
 * @brief Sets bit at position
 * with given value
 * 
 * @param to destination
 * @param val value of bit
 * @param pos position to update
 */
void setBitAt(dword *to, byte val, byte pos);

/**
 * @brief Converts
 * hexadecimal bytearray to string
 * 
 * @param bytearray input
 * @param buff output
 */
void hexToString(dword bytearray, char buff[]);

/**
 * @brief Checks parity of 64b given key and builds new
 * 
 * @param k to be checked
 * @return parity key
 */
dword build_parity_key(dword k);

/**
 * @brief Checks parity of given
 * key
 * 
 * @param k key
 * @return {1: odd parity; 0: even parity} 
 */
byte check_dword_parity(dword k);

/**
 * @brief Checks parity of given byte
 * 
 * @param b byte to check
 * @return {1: odd; 0: even}
 */
byte check_byte_parity(byte b);

#endif