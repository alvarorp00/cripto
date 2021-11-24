/**
 * @file linearity_main.c
 * @author your name (you@domain.com)
 * @brief Study the non-linearity of the
 * S-Boxes for DES algorithm.
 * @version 0.1
 * @date 2021-11-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>

#include "params.h"
#include "logger.h"
#include "des.h"
#include "sodium.h"

/**
 * @brief We're using our
 * _f function used previously in
 * DES.
 * 
 * @param r 
 * @param sbk 
 * @return word 
 */
extern word _f(word r, union bconv_t sbk);

/**
 * @brief Performs
 * hamming distance between
 * two numbers given
 * 
 * See: https://en.wikipedia.org/wiki/Hamming_distance
 * 
 * @param s1 
 * @param s2 
 * @return {byte} hamming distance
 */
static byte hamming(dword s1, dword s2);

const char *__info    = "\nThis test will try to prove that, given f that is function\n\r\
                        \rused in DES, given a 32b block (word) and a 48b block(key)\n\r\
                        \r- notice key is a subkey expanded previously - proceeds as\n\r\
                        \rfollows:\n\r\
                        \r\t-> Expands 32b word into 48b\n\
                        \r\t-> xor's it with given 48b of the key\n\
                        \r\t-> Now that we have 8 blocks of 6b each:\n\
                        \r\t\t-> Using s-boxes, we pass it onto 8 blocks of 4b each\n\
                        \r\t-> 32b output is generated. This is mixed once again\n\r\
                        \r\t   with a permutation and then is returned.\n\n\
                        \rsatisfies theorem.\n\r\
                        \r\n\
                        \r\n";

const char *__theorem = "\nTheorem:\n\r\
                         \r   Non-linearity is one of the principles of design of\n\r\
                         \r   DES cipher, and assures the next:\n\r\
                         \r          f (x ⊕ y) ≠ f(x) ⊕ f(y)\n\r\
                         \r\n\
                         \r   We are performing the next:\n\r\
                         \r      -> generate word (32b) and key (48b) randomly, then compute statistics generated\n\r\
                         \r      -> f is a boolean function in 2 variables:\n\r\
                         \r         so we could approximate this using bent functions theorem.\n\r\
                         \r\nSee memory given for further info here (this test does not include it).\n\r\n";

#define __G_ROUNDS 8192

struct _matches_t {
  word  r_block1; // msg block
  word  r_block2; // msg block
  dword k_block; // key generated
  byte  hdistance; // hamming distance for f()
  byte  blockdistance; // hamming distance for r1 & r2 
};

void swap (void *a_ptr, void *b_ptr, size_t size);
int32_t partition(struct _matches_t *data, int32_t low, int32_t hight);
void quickSort (struct _matches_t *data, int32_t low, int32_t hight );

void matches_test();

/** ********************************* **/
/** ********************************* **/
/** ************* MAIN ************** **/
/** ********************************* **/
/** ********************************* **/

int main(int argc, char **argv)
{

  matches_test(); // run matches test
  
  return 0;
}

/** ********************************* **/
/** ********************************* **/
/** ************* FNCTS ************* **/
/** ********************************* **/
/** ********************************* **/

void swap (void *a_ptr, void *b_ptr, size_t size)
{
  void *tmp = malloc(size);
  assert( tmp != NULL);
  memcpy(tmp, a_ptr, size);
  memcpy(a_ptr, b_ptr, size);
  memcpy(b_ptr, tmp, size);
  free(tmp);
}

/**
 * Purely base on geek for geeks quick implementation.
 * 
 * See: https://www.geeksforgeeks.org/quick-sort/
 */
int32_t partition(struct _matches_t *data, int32_t low, int32_t hight)
{
  byte pivot;
  int32_t i, j;

  pivot = data[hight].hdistance;
  i = low - 1;

  for (j=low; j <=hight - 1; j++)
  {
    if (data[j].hdistance < pivot)
    {
      i++;
      swap(&(data[i]), &(data[j]), sizeof(struct _matches_t));
    }
  }
  swap (&(data[i+1]), &(data[hight]), sizeof(struct _matches_t));
  return (i+1);
}

void quickSort (struct _matches_t *data, int32_t low, int32_t hight )
{
  uint16_t pi;
  if (data && low < hight)
  {
    pi = partition(data, low, hight);

    quickSort(data, low, pi -1);
    quickSort(data, pi + 1, hight);
  }
}

static byte hamming(dword s1, dword s2)
{
  dword xored = 0;

  byte h      = 0;
  byte i      = 0;

  xored = s1 ^ s2;

  for (i=0; i<64; i++)
    if (bitAt(xored, i))
      h++;
  
  return h;
}

/** ********************************* **/
/** ********************************* **/
/** ************* TEST ************** **/
/** ********************************* **/
/** ********************************* **/

void matches_test()
{
  size_t  i      = 0; // counter
  
  LOG_INFO("%s", __info);
  LOG_INFO("%s", __theorem);

  LOG_INFO("Generic test. Random data & statistics then...\n");

  assert(sodium_init() >= 0);

  word   r_block1  = 0; // 32b msg
  word   r_block2  = 0; // 32b msg

  dword  k_block   = 0; // 48b key, same for both
  union bconv_t kc = {0x00}; // b64 <-> bytearray conversion param

  word   f_retr1   = 0; // return of f function of r1
  word   f_retr2   = 0; // return of f function of r2
  word   f_retrx   = 0; // return of f function of r1 xor r2

  struct _matches_t *matches = (struct _matches_t*)calloc(__G_ROUNDS, sizeof(struct _matches_t));
  assert(matches != NULL);

  size_t match_count         = 0;

  for (i=0; i<__G_ROUNDS; i++)
  {
    r_block1 = randombytes_random();
    r_block2 = randombytes_random();

    k_block  = ( ( ( (dword)randombytes_random() & 0xFFFF ) << 32) | ( randombytes_random() ) );
    kc.l = k_block;

    // LOG_INFO("r1 (a): %lx;\nr2 (b): %lx\n; k (k): %lx\n", r_block1, r_block2, k_block);

    f_retr1 = _f(r_block1, kc);
    f_retr2 = _f(r_block2, kc);

    f_retrx = _f( ( r_block1 ^ r_block2 ), kc );

    matches[i].k_block       = k_block;
    matches[i].r_block2      = r_block2;
    matches[i].r_block1      = r_block1;
    matches[i].hdistance     = hamming(f_retrx, f_retr1 ^ f_retr2 );
    matches[i].blockdistance = hamming(r_block1, r_block2);
  }

  quickSort(matches, 0, __G_ROUNDS - 1);
  double avgd = 0.0; // average distance

  LOG_INFO("Test finished. Printing results...\n");
  for (i=0; i<__G_ROUNDS; i++)
  {
    avgd += matches[i].hdistance;
    if (matches[i].hdistance == 0)
      match_count++;
  }
  avgd /= __G_ROUNDS;

  LOG_INFO("Average distance has been: %lf\n", avgd);
  LOG_INFO("Number of exact matches (distace == 0): %ld\n", match_count);
  LOG_INFO("Tests done: %d rounds.\n", __G_ROUNDS);


  int16_t n = -1;
  do
  {
    LOG_INFO("Print n-th distances given [introduce n]: ");
    scanf("%hd", &n);
    
    LOG_INFO("\t->Distance [nº%hd]: %d\n", n, matches[n].hdistance);
    LOG_INFO("\t\t->hamming(r1, r2) = %d\n", matches[n].blockdistance);
    LOG_INFO("\t\t->r1_32b: %x\n", matches[n].r_block1);
    LOG_INFO("\t\t->r2_32b: %x\n", matches[n].r_block2);
    LOG_INFO("\t\t->ke_48b: %lx\n", matches[n].k_block);
    if (matches[n].hdistance == 0 )
    {
      LOG_INFO("\t\t ->CLUE! Match with (r1 | a: %x), (r2 | b: %x)\n", matches[n].r_block1, matches[n].r_block2);
    }

  } while (n<8192 && n>=0);
  

  
}