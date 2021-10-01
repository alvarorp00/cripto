/**
 * @file argparse.c
 * @author your name (alvarorp00@sigsuspend.net)
 * @brief Simple module for parsing args
 * @version 0.1
 * @date 2021-09-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "argparse.h"

#define P_ARGS(p) (p)->args
#define P_ARGS_AT(p, i) (p)->args[i]
#define P_MXSZ(p) (p)->maxsize
#define P_CURRSZ(p) (p)->currsize
#define P_DESCRP(p) (p)->description

#define A_NAME(a) (a)->argname
#define A_PATTERN(a) (a)->argpattern
#define A_NARGS(a) (a)->nargs
#define A_HELP(a) (a)->help
#define A_TYPE(a) (a)->type
#define A_QUEUE(a) (a)->arg_q

#define _INIT_PRSR_SZ 64
#define _BUFF 256 + 1

typedef struct _Argument Argument;

struct _Argument {
  char *argname;
  char *argpattern;
  uint8_t nargs;
  char *help;
  ArgType type;
  queue_t *arg_q;
};

struct _Parser {
  Argument **args;
  uint_fast64_t maxsize;
  uint_fast64_t currsize;
  char description[_BUFF*4];
};

Parser* argparse_init()
{
  Parser *p;

  p = (Parser*)malloc(sizeof(Parser));

  if (!p)
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  P_MXSZ(p) = _INIT_PRSR_SZ;
  P_CURRSZ(p) = 0;

  P_ARGS(p) = (Argument**)calloc(_INIT_PRSR_SZ, sizeof(Argument*));

  if (!P_ARGS(p))
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  return p;
}

bool argparse_set_descr(Parser *parser, char *descrp)
{
  if (!parser || !descrp)
  {
    #line __LINE__ __FILE__
    return false;
  }

  strncpy(P_DESCRP(parser), descrp, _BUFF*4);

  return true;
}

bool argparse_add_argument(
  Parser *parser,
  char *argname,
  ArgType type,
  char *pattern,
  uint8_t nargs,
  char *help
)
{
  Argument *new;

  if (!parser || !argname || !pattern)
  {
    #line __LINE__ __FILE__
    return false;
  }

  if (P_CURRSZ(parser) == P_MXSZ(parser))
  {
    #line __LINE__ __FILE__
    return false;
  }

  new = (Argument*)malloc(sizeof(Argument));

  if (!new)
  {
    #line __LINE__ __FILE__
    return false;
  }

  A_NAME(new) = argname;
  A_PATTERN(new) = pattern;
  A_TYPE(new) = type;
  A_NARGS(new) = nargs;
  A_HELP(new) = help;
  A_QUEUE(new) = queue_init(NULL, NULL, (free_proto)free, NULL); // storing dynamic char buffers -> classic free function

  P_ARGS_AT(parser, P_CURRSZ(parser)++) = new;

  // if ((P_CURRSZ(parser)++) == P_MXSZ(parser))
  // {
  //   //TODO REALLOC
  // }

  return true;
}

/**
 * @brief Parses input args from
 * previous configuration that's been specified.
 * 
 * It follows linear probing and it's not efficient,
 * cause arguments are as much 8 or 9 so it's not neccessary
 * to get better performance at this point.
 * 
 * @param parser 
 * @param argc 
 * @param argv 
 * 
 * @return status
 */
bool argparse_parse_args(Parser *parser, int argc, const char *argv[])
{
  Argument *arg;
  char *buff;
  size_t i, j, k;
  
  if (!parser || !argv)
  {
    #line __LINE__ __FILE__
    return false;
  }

  if (P_ARGS(parser) == NULL)
  {
    #line __LINE__ __FILE__
    return false;
  }

  for (i = 0; i < argc; i++)
  {
    for (j = 0; j < P_CURRSZ(parser); j++)
    {
      if ((arg = P_ARGS_AT(parser, j)) == NULL)
        continue;
      if (strncmp(argv[i], A_PATTERN(arg), _BUFF)) // != 0 -> NOT A MATCH -> Continue
        continue;
      // Pattern Match!
      if (A_TYPE(arg) == EMPTY)
        continue;
      for (k = 1; k <= A_NARGS(arg); k++)
      {
        buff = (char*)calloc(_BUFF, sizeof(char));
        if (!buff)
        {
          #line __LINE__ __FILE__
          return false;
        }
        strncpy(buff, argv[i+k], _BUFF);
        queue_insert(A_QUEUE(arg), buff); // trust this won't fail
      }
      i += k;
      break; // argument with same pattern won't be read nor stored
    }
  }

  return true;
}

queue_t *argparse_get_args(Parser *p, char* argname)
{
  size_t i;
  
  if (!p || !argname)
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  for (i = 0; i < P_CURRSZ(p); i++)
  {
    if (strncmp(A_NAME(P_ARGS_AT(p, i)), argname, _BUFF) == 0) // match!
    {
      return A_QUEUE(P_ARGS_AT(p, i));
    }
  }

  return NULL;
}

void argparse_clean(Parser *p)
{
  size_t i;
  
  if (!p)
    return;

  if (P_ARGS(p) != NULL)
  {
    for (i = 0; i < P_CURRSZ(p); i++)
    {
      if (P_ARGS_AT(p, i) != NULL)
      {
        queue_clean(A_QUEUE(P_ARGS_AT(p,i)));
        free (P_ARGS_AT(p, i));
      }
    }
    free(P_ARGS(p));
  }

  free(p);

}

#ifdef __DEBUG__
void argparse_print_args(Parser *p, FILE *out)
{
  char *buff;
  size_t i;
  
  if (!p)
    return;

  TO_FILE(out, "@@@ DEBUG @@@ -> Printing argparse info\n");

  for (i = 0; i < P_CURRSZ(p); i++)
  {
    TO_FILE(out, "\t -> Argument %ld out of %ld || Name: %s \n", i + 1, P_CURRSZ(p), A_NAME(P_ARGS_AT(p,i)));
    TO_FILE(out, "\t\t - Pattern: %s\n", A_PATTERN(P_ARGS_AT(p, i)));
    TO_FILE(out, "\t\t - Args:\n");
    while(!queue_isEmpty(A_QUEUE(P_ARGS_AT(p, i))))
    {
      buff = (char*)queue_extract(A_QUEUE(P_ARGS_AT(p,i)));
      TO_FILE(out, "\t\t\t - %s\n", buff);
    }
    if (A_HELP(P_ARGS_AT(p, i)) != NULL)
    {
      TO_FILE(out, "\t\t - Help: %s\n", A_HELP(P_ARGS_AT(p, i)));
    }
  }
}
#endif