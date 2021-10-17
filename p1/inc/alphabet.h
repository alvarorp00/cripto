#ifndef __ALPHABET_H__
#define __ALPHABET_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct _alphabet_t alphabet_t;

#define NEXT(n) (n)->next

struct AlphabetIterator
{
  enum LangMode {CASTILLIAN = 0, ENGLISH} langmode;
  struct ApIteratorNode {
    struct ApIteratorNode *next; // points next or NULL if it's last one
    struct ApIteratorNode *last; // points last or NULL if it0s first one
    char chr; // correspondant character
    union Prob
    {
      float cast; // probability of occurrence in castillian
      float eng; // probability of ocurrence in english
    } prob; 
  } *node;
  bool ok;
};

alphabet_t *alphabet_init(size_t a_size);

bool alphabet_map(alphabet_t *alphabet, char c, int_fast8_t n);

char alphabet_get_fromNum(alphabet_t *alphabet, int_fast8_t n);

int_fast8_t alphabet_get_fromChar(alphabet_t *alphabet, char c);

bool alphabet_loadFromFile(alphabet_t *alphabet, const char *filename);

bool alphabet_contains_num(alphabet_t *alphabet, int_fast8_t num);

bool alphabet_contains_chr(alphabet_t *alphabet, char c);

int_fast8_t alphabet_getCurrentSize(alphabet_t *alphabet);

/**
 * @brief Offset to first character if
 * we're considering values starting in 0 and
 * alphabet does not
 * 
 * @param alphabet 
 * @return int_fast8_t 
 */
int_fast8_t alphabet_get_offset(alphabet_t *alphabet);

void alphabet_clean(alphabet_t *alphabet);

/**
 * @brief Returns structure
 * containing nodes sorted by frequency
 * by desired mode
 * 
 * @param alphabet needed 
 * @param mode CASTILLIAN or ENGLISH
 * @return struct AlphabetIterator  with sorted list node
 */
struct AlphabetIterator *alphabet_sortByFreq(alphabet_t *alphabet, enum LangMode mode);

struct ApIteratorNode *alphabet_iteratorFreqAt(struct AlphabetIterator *iterator, size_t idx);

/**
 * @brief Cleans memory alloc'd
 * by structure provided
 * 
 * @param afsort struct param to dealloc
 */
void alphabet_iteratorFree(struct AlphabetIterator *afsort);

/**
 * @brief Prints alphabet
 * at current status
 * 
 * @param alphabet to print
 * @param dest destination file
 * @return size_t total bytes printed
 */
size_t alphabet_print(alphabet_t *alphabet, FILE *dest);

#endif