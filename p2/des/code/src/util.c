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

#include <string.h>
#include <sodium.h>

#include "util.h"

uint32_t dump_to_file(FILE *o_file, char* msg, size_t size)
{
  uint32_t bytes;
  size_t i;
  
  if (!o_file || !msg)
    return 0;

  for (i=0; i<size; i++)
  {
    bytes += fprintf(o_file, "%c", msg[i]);
  }

  return bytes;
}

char* load_all_from_file(FILE *i_file)
{
  size_t  max = KB1, // initial maximum length
          len = 0; // current offset
  
  char* input;
  char c;
  
  if (!i_file)
    return NULL;
  
  input = (char*)calloc(KB1, sizeof(char));
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
    input[len] = c;

    if (++len == max)
    {
      // expand input size
      input = realloc(input, (max <<= 1) * sizeof(char));
    }
  }

  input = realloc(input, (len + 1) * sizeof(char)); // truncate
  input[len] = 0;

  fseek( i_file, 0, SEEK_SET ); // preserve i_file

  return input;
}

dword string8ToB64(byte msg[])
{
  dword _msg  = 0;
  byte i      = 0;
  byte sz     = 0;

  if (!msg)
    return 0x00;

  sz = strlen((char*)msg);

  for (i=0; i < 8 && i < sz; i++)
  {
    _msg <<= 8;
    _msg |= (dword)msg[i];
  }
  return _msg;
}

dword get_random_key()
{
  if (sodium_init() < 0)
  {
    return 1; // library couldn't be initialized. Fatal error.
  }

  return ((((dword)randombytes_random()) << 32) | randombytes_random() );
}

dword reverse(const dword n, const dword k)
{
  dword r, i;
  for (r = 0, i = 0; i < k; ++i)
          r |= ((n >> i) & 1) << (k - i - 1);
  return r;
}

byte bitAt(dword from, byte pos)
{
  return (from >> pos) & 1;
}

void setBitAt(dword *to, byte val, byte pos)
{
  *(to) = (((*(to) | (1 << pos)) ^ (1 << pos))) | (val << pos);
}

void hexToString(dword bytearray, char *buff)
{
  union bconv_t conv;
  byte i;

  if (!buff || strlen(buff) < 8)
    return;

  conv.l = bytearray;
  for (i=0; i<8; i++)
    buff[i] = (char)conv.bytes[i];
}

dword build_parity_key(dword k)
{
  dword _k = 0;
  dword  b;
  byte      i;

  for (i=0; i<8; i++)
  {
    b = ((k >> (8 * i)) & 0x7F); /* Map this byte */
    if (check_byte_parity((byte)b) == 0) /* mask with 0xFF already done */
      _k |= ((b |= 0x80) << (8 * i));
    else
      _k |= (b << (8 * i));
  }

  return _k;
}

byte check_dword_parity(dword k)
{
  dword  b;
  byte   i;

  for (i=0; i<8; i++)
  {
    b = ((k >> (8 * i)) & 0xFF); /* Map this byte */
    if (check_byte_parity((byte)b) == 0) /* mask with 0xFF already done */
      return 0;
  }

  return 1;
}

byte check_byte_parity(byte b)
{
  byte p = 0;
  byte _b = b;

  while (_b)
  {
    p = !p;
    _b = _b & (_b - 1);
  }

  return p;
}