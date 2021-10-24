#ifndef __CIPHER_H__
#define __CIPHER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "gmp.h"
#include "alphabet.h"

#define _DICT_FNAME "config/dictionary.json"

enum OPTION {
  CIPHER, DECIPHER
};

#define ERRBUFF_LEN 1024

extern char errbuff[ERRBUFF_LEN + 1];
extern bool cipher_status;

/**
 * @brief Affine cipher using
 * numerical procedures of multiple precision
 * defined in GNU MP (GMP).
 * 
 * Verifies that both numbers given, a & b,
 * determine an affine inyective function.
 * If they not, it'll raise an error.
 * 
 * If input file is not given, standard input will be held until
 * data is provided.
 * 
 * If output file is not given, standard output will be used.
 * 
 */
void affine(enum OPTION opt, const char *m, const char *a, const char *b, FILE *i_file, FILE *o_file);

void affine_modified(enum OPTION opt, const char *m, char **a, char **b, uint8_t klength, FILE *i_file, FILE *o_file);

/**
 * @brief Performs a known cipher text
 * attack to above's affine cipher
 * modification.
 * 
 * @param m 
 * @param i_file 
 * @param o_file 
 */
void affine_mod_cryptanalyze(const char *m, FILE *i_file, FILE *o_file);

/**
 * @brief Vigenere's cipher
 * 
 * @param opt cipher or decipher
 * @param m alphabet_sz
 * @param keystring cipher keystring
 * @param i_file input file
 * @param o_file output file
 */
void vigenere(enum OPTION opt, const char *m, char *keystring, FILE *i_file, FILE *o_file);

#endif