#ifndef __ALPHABET_H__
#define __ALPHABET_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct _alphabet_t alphabet_t;

alphabet_t *alphabet_init(size_t a_size);

bool alphabet_map(alphabet_t *alphabet, char c, int_fast8_t n);

char alphabet_get_fromNum(alphabet_t *alphabet, int_fast8_t n);

int_fast8_t alphabet_get_fromChar(alphabet_t *alphabet, char c);

bool alphabet_loadFromFile(alphabet_t *alphabet, const char *filename);

bool alphabet_contains_num(alphabet_t *alphabet, uint8_t num);

bool alphabet_contains_chr(alphabet_t *alphabet, char c);

void alphabet_clean(alphabet_t *alphabet);

size_t alphabet_print(alphabet_t *alphabet, FILE *dest);

#endif