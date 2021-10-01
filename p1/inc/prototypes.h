#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include <stdio.h>
#include <stdint.h>

typedef int (*cmp_proto) ( void*, void* );
typedef void* (*cpy_proto) ( void* );
typedef void (*free_proto) ( void* );
typedef void (*print_proto) ( FILE*, void* );

/**
 * hashcode_t Funct prototype
 * @param void elem to be compared
 * @return hashcode value
 */
typedef uint_fast64_t (*hashcode_t)(void*);

/**
 * equals_t Func prototype
 * @param void element 1
 * @param void element 2
 * @return Returns if both elements are equal
 */
typedef bool (*equals_t)(void*, void*);

/**
 * clean_t Func prototype
 * @param void element to clean
 */
typedef void (*clean_t)(void*);

#endif