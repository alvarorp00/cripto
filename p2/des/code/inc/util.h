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

typedef char byte;
typedef byte* byte_ptr;

#define KB1 1024
#define MB1 KB1 * KB1
#define GB1 MB1 * KB1 // too big!

#define LCHAR 65
#define HCHAR 90

/**
 * @brief Loads data stored inside 
 * given file starting in given offset.
 * Amount read is specified in blocksz.
 * Preserves file pointer performing fseek
 * to starting point of file before and after
 * searching data.
 * 
 * Please, for using stdin give a zero-offset
 * 
 * @param i_file input file
 * @param blocksz maximum data read
 * @param offset shift position
 * @return char* 
 */
byte_ptr load_from_file(FILE *i_file, size_t blocksz, size_t offset);

/**
 * @brief Loads all data stored inside
 * given file until EOF.
 * 
 * @param i_file input file
 * @return data read
 */
byte_ptr load_all_from_file(FILE *i_file);

#endif