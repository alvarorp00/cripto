/**
 * @file util.c
 * @author your name (you@domain.com)
 * @brief Definitions of helper functions
 * used in des algorithm
 * @version 0.1
 * @date 2021-11-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <unistd.h>
#include "util.h"

byte_ptr load_from_file(FILE *i_file, size_t blocksz, size_t offset)
{
  size_t len = 0; // current data read
  
  byte_ptr input;
  byte c;
  
  if (!i_file)
    return NULL;
  
  input = (byte_ptr)calloc(blocksz, sizeof(byte));
  if (!input)
    return NULL;

  // seek offset
  fseek(i_file, offset, SEEK_CUR);

  // read plain text

  if (i_file == stdin)
  {
    printf("-> Enter message (press CTRL + D in new line to finish): \n");
  }

  while((c = (byte)fgetc(i_file)) != EOF)
  {
    // scape conditions for some characters...
    if (c < LCHAR || c > HCHAR)
      continue;

    input[len++] = c;

    if (len == blocksz)
      break;
  }

  while(len<blocksz)
    input[len++] = 0;

  fseek( i_file, 0, SEEK_SET ); // preserve i_file

  return input;
}

byte_ptr load_all_from_file(FILE *i_file)
{
  size_t  max = KB1, // initial maximum length
          len = 0; // current offset
  
  byte_ptr input;
  byte c;
  
  if (!i_file)
    return NULL;
  
  input = (byte_ptr)calloc(KB1, sizeof(byte));
  if (!input)
    return NULL;

  // seek start
  if (ftell(i_file) != 0)
    fseek(i_file, 0, SEEK_SET);

  // read plain text

  if (i_file == stdin)
  {
    printf("-> Enter message (press CTRL + D in new line to finish): \n");
  }

  fseek(i_file, 0, SEEK_SET); // move to starting point of file

  while((c = fgetc(i_file)) != EOF)
  {
    // scape conditions for some characters...
    if (c < LCHAR || c > HCHAR)
      continue;

    input[len] = c;

    if (++len == max)
    {
      // expand input size
      input = realloc(input, (max <<= 1) * sizeof(byte));
    }
  }

  input = realloc(input, (len + 1) * sizeof(byte)); // truncate
  input[len] = 0;

  fseek( i_file, 0, SEEK_SET ); // preserve i_file

  return input;
}