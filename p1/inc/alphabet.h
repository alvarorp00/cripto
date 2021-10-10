#ifndef __ALPHABET_H__
#define __ALPHABET_H__

#include <stdio.h>
#include <stdint.h>

typedef struct _alphabet_t alphabet_t;

alphabet_t *alphabet_init(const char *filename);

const char *alphabet_get_fromNum(alphabet_t *alphabet, int_fast8_t n);

int_fast8_t alphabet_get_fromChar(alphabet_t *alphabet, const char c);

void alphabet_clean(alphabet_t *alphabet);

#endif