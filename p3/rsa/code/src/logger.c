#include "logger.h"

#include <math.h>
#include <stdint.h>
#include <sys/random.h>

int __log_level__ = 0;

void logger_log_level( int level ) {
  if ( level < 0 ) level = 0;
  __log_level__ = level;
}

int logger_random_btw( int a, int b ) {

  double coe;
  uint8_t __buff[ sizeof(int) ];
  static unsigned int max = (unsigned int)~0;
  
  getentropy( __buff, sizeof( __buff ) );
  
  unsigned int r = *((int*)__buff);

  coe = r / ((double)max);

  return (int)(a + (b-a+1)*coe);
}

void logger_reduce_str( const char *filter, char *src, int *len ) {

  if ( !filter || !src ) return;

  int __filter_map[256] = {0};
  
  // fill up buff table
  while ( *filter ) __filter_map[*filter++]++;

  *len = 0;
  char *oldPtr = src;

  while (*src) {
    
    if ( __filter_map[*src] == 0 ) {

      *oldPtr++ = *src;
      (*len)++;

      /*
      if ( spaceMap && spaceSize ) {
        if ( spaceMap[*src] == 0 ) {
          (*spaceSize)++;
          spaceMap[*src] = 1;
        }
      }
      */

    }

    src++;
  }

  *oldPtr = 0;

}
