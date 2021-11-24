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

const char *__info = "\nStrict Avalanche Criteria (Theorem):\n\
                      \n\rAn output bit from SBoxes will change\n\
                      \rwith 1/2 probability when an input bit is complemented.\n\
                      \n\r\t let a = a1a2a3a4a5a6\n\r\
                      \r\t let b = b1b2b3b4 <- SBox(a)\n\
                      \n\r\t P(bj = 1 | /ai) = P(bj = 0 | /ai) = 1/2.\n\r";

const char *__extra__info = "With previous information, if we calculate the times bj=1 & bj=0 occur,\n\r\
                             \rwe'll see that they must be quite similar. This means the next: if we do the factor between them, then:\n\r\
                             \r\n\t(times(b_j=0) / times(b_j=1)) ~ 1\n\
                             \r\nWe'll be doing 4096 iteratios, each of them will toggle each bit in 6b sbox input, and then check 4b output values\n\
                             \rto see if they're either 1 or 0.\r\n";

/** ********************************* **/
/** ********************************* **/
/** ************* MAIN ************** **/
/** ********************************* **/
/** ********************************* **/

#define __A_ROUNDS    4096 // avalanche rounds
#define __T_ROUNDS    6 // toggle rounds
#define __B_ROUNDS    4 // output is of 4b
#define __S_ROUNDS    8 // 8 SBOXES

void toggleBitRandom(dword *block, byte upperbound);
void toggleBitAt    (dword *block, byte pos       );

#define S_BOX_AT(sbox, row, column) S_BOXES[sbox][row][column]

#define SIXB_MSK(b) (b & 0x3F)
#define SIXB_ROW(b) ( ( (b & 0x20) >> 5 ) | ( b & 0x01 ) )
#define SIXB_CLM(b) ( ( b & 0x1E ) >> 1 )

typedef struct
{
  uint64_t changes1[4]; // count amount of ones for each bit
  uint64_t changes0[4]; // count amount of zeros for each bit
  float    eq      [4]; // factor between changes1 and changes0
}sbox_matches;


int main(int argc, char const *argv[])
{ 
  dword  block       = 0x00; // 48b block
  dword  sixb        = 0x00; // storing 6 bits
  dword  fourb       = 0x00; // storing 4 bits
  
  size_t a, i, j, k;
  
  sbox_matches matches = {0x00};

  LOG_INFO("%s\n", __info);
  LOG_INFO("%s\n", __extra__info);
  
  assert(sodium_init() >= 0);

  for ( a=0; a<__A_ROUNDS; a++ )
  {
    block = ( ( (dword)( ( randombytes_random() & 0xFFFF ) ) << 32 ) | randombytes_random() );
    
    for ( i=0; i<__S_ROUNDS; i++ ) // we must initialize output
    {
      // initial load
      sixb  = SIXB_MSK( (block >> (6*i)) );

      for ( j=0; j<__T_ROUNDS; j++ ) // available toggle rounds!
      {
        toggleBitAt(&(sixb), j);
        fourb  = S_BOX_AT(i, SIXB_ROW(sixb), SIXB_CLM(sixb));
        
        for ( k=0; k<__B_ROUNDS; k++ )
        {
          if (bitAt(fourb, k) == 0)
          {
            matches.changes0[k] += 1;
          }
          else
          {
            matches.changes1[k] += 1;
          }
        }
      }
    }
  }

  // matches.tprob = 0;
  for ( i=0; i<__B_ROUNDS; i++)
    matches.eq[i] = ((double)matches.changes1[i]) / ((double)matches.changes0[i]);

  for ( i=0; i<__B_ROUNDS; i++)
  {
    LOG_INFO("Values for b%ld:\n", i+1);
    LOG_INFO("\t P[b%ld=0] = %f\n", i+1, ((float)matches.changes0[i])/(__A_ROUNDS*__S_ROUNDS*__T_ROUNDS));
    LOG_INFO("\t P[b%ld=1] = %f\n", i+1, ((float)matches.changes1[i])/(__A_ROUNDS*__S_ROUNDS*__T_ROUNDS));
    LOG_INFO("\t times(b%ld=1) / times(b%ld=0) = %f\n", i+1, i+1, matches.eq[i]);
  }

  // LOG_INFO("Prob. of changing: %lf\n", matches.tprob);
  
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