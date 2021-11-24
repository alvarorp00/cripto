/**
 * @file avalanche_main.c
 * @author your name (you@domain.com)
 * @brief performs a bunch of
 * test checking avalanche criteria of
 * DES algorithm 
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

const char *__info = "\nStrict Avalanche Criteria (Theorem):\n\
                      \n\rAn output bit from SBoxes will change\n\
                      \rwith 1/2 probability when an input bit is complemented.\n\
                      \n\r\t let a = a1a2a3a4a5a6\n\r\
                      \r\t let b = b1b2b3b4 <- SBox(a)\n\
                      \n\r\t P(bj = 1 | /ai) = P(bj = 0 | /ai) = 1/2.\n\r";

const char *__extra__info = "\nConsider a bent function (in combinatory, a boolean\n\r\
                             \rfunction with N parameters that is maximally non-linear); \n\r\
                             \rit's different as most from all linear & affine function's set\n\r\
                             \rwhen measured with hamming distance between truth tables.\n\r\
                             \r\nThis puts as in the next:\n\n\
                             \r  -> A bent function is a boolean function in n variables (n is even)\n\r\
                             \r     such that for any nonzero vector y, it's derivative Dy(f(x)) = f(x) xor f (x xor y) is balanced,\n\r\
                             \r     which means that takes values of 1 or 0 equally often [Definition 2].\n\r\
                             \r\nSee https://www.sciencedirect.com/topics/mathematics/bent-function for further info.\n\r";


/** ********************************* **/
/** ********************************* **/
/** ************* MAIN ************** **/
/** ********************************* **/
/** ********************************* **/

#define __A_ROUNDS    8192 // avalanche rounds
#define __T_ROUNDS    32 // toggle rounds
#define __S_ROUNDS    8 // 8 SBOXES

void toggleBitRandom(dword *block, byte upperbound);
void toggleBitAt    (dword *block, byte pos       );

#define S_BOX_AT(sbox, row, column) S_BOXES[sbox][row][column]

#define SIXB_MSK(b) (b & 0x3F)
#define SIXB_ROW(b) ( ( (b & 0x20) >> 5 ) | ( b & 0x01 ) )
#define SIXB_CLM(b) ( ( b & 0x1E ) >> 1 )

typedef struct
{
  byte   changes[32]; // count amount of changes for each bit
  double pprobs [32]; // probability of each bit of changing it's value
  double tprob      ; // normalized probability of a bit changing it's value
}avalanche_matches_t;


int main(int argc, char const *argv[])
{
  // des_t *cipher = NULL; // des cryptogram
  
  dword  block       = 0x00; // 48b block
  word   output      = 0x00; // 32b output for each round
  word   prev_out    = 0x00; // previous round 32b output
  byte   sixb        = 0x00; // storing 6 bits
  byte   fourb       = 0x00; // storing 4 bits
  size_t acc         = 0x00; // count total difference found

  byte   diff        = 0x00; // hamming result
  
  size_t i, j;
  
  avalanche_matches_t matches = {0x00};

  LOG_INFO("%s\n", __info);
  // LOG_INFO("%s\n", __extra__info);
  
  assert(sodium_init() >= 0);

  block = ( ( (dword)( ( randombytes_random() & 0xFFFF ) ) << 32 ) | randombytes_random() );

  /**
   * We're starting with a 48b random seed
   * for block.
   * 
   * We then do an initial partialization of
   * it computing an output so
   * then we can toggle a bit and check
   * those bits that would've changed
   * in every of the __A_ROUND (1 - 32)
   * 
   * Should be ~32, checking with previous
   * output using hamming distance
   */

  for ( i=0; i<__S_ROUNDS; i++ ) // we must initialize output
  {
    sixb   = SIXB_MSK( (block >> (6*i)) );
    fourb  = S_BOX_AT(i, SIXB_ROW(sixb), SIXB_CLM(sixb));
    output <<= 4;
    output |= fourb;
  }

  for ( i=0; i<__T_ROUNDS; i++ )
  {
    prev_out = output; // preserve for comparison
    toggleBitAt(&(block), i); // toggle value
    output = 0x00; // clean previous data
    for ( j=0; j<__S_ROUNDS; j++ )
    {
      sixb   = SIXB_MSK( (block >> (6*j)) );
      fourb  = S_BOX_AT(j, SIXB_ROW(sixb), SIXB_CLM(sixb));
      output <<= 4;
      output |= fourb;
    }

    // recalculate differences
    for ( j=0; j<__T_ROUNDS; j++ )
    {
      if (bitAt(prev_out, j) != bitAt(output, j))
      {
        
      }
    }
    
  }

  
  return 0;
}

/** ********************************* **/
/** ********************************* **/
/** ************* FNCTS ************* **/
/** ********************************* **/
/** ********************************* **/

void toggleBitRandom(dword *block, byte upperbound)
{
  byte pos = 0, new;
  if (!block)
    return;
  pos = randombytes_uniform(upperbound); // rnd between 0 and upperbound (not included)
  new = (bitAt(*(block), pos)) ? 0 : 1; // toggle value
  setBitAt(block, new, pos);
}

void toggleBitAt (dword *block, byte pos)
{
  byte new;
  if (!block)
    return;
  new = (bitAt(*(block), pos)) ? 0 : 1; // toggle value
  setBitAt(block, new, pos);
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