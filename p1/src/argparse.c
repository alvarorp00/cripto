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
#define A_PARAM(a) (a)->param

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
  char *param; // Just for single type arguments
};

struct _Parser {
  Argument **args;
  uint_fast64_t maxsize;
  uint_fast64_t currsize;
  char *description;
};

Parser* argparse_init()
{
  Parser *p = NULL;
  size_t i;

  p = (Parser*)calloc(1, sizeof(Parser));

  if (!p)
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  P_MXSZ(p) = _INIT_PRSR_SZ;
  P_CURRSZ(p) = 0;

  P_ARGS(p) = NULL;
  P_ARGS(p) = (Argument**)calloc(_INIT_PRSR_SZ, sizeof(Argument*));

  if (!P_ARGS(p))
  {
    #line __LINE__ __FILE__
    return NULL;
  }

  for (i = 0; i < _INIT_PRSR_SZ; i++)
  {
    P_ARGS_AT(p, i) = NULL;
  }

  P_DESCRP(p) = (char*)calloc(_BUFF*4, sizeof(char));
  
  if (!P_DESCRP(p))
  {
    #line __LINE__ __FILE__
    // TRACE()
    P_DESCRP(p) = NULL;
  }

  return p;
}

bool argparse_set_descr(Parser *parser, const char *descrp)
{
  if (!parser || !descrp)
  {
    #line __LINE__ __FILE__
    return false;
  }

  if (P_DESCRP(parser) == NULL)
  {
    return false;
  }

  strncpy(P_DESCRP(parser), descrp, _BUFF*4);

  return true;
}

bool argparse_add_argument(
  Parser *parser,
  const char *argname,
  ArgType type,
  const char *pattern,
  uint8_t nargs,
  const char *help
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

  new = (Argument*)calloc(1, sizeof(Argument));

  if (!new)
  {
    #line __LINE__ __FILE__
    goto add_argument_failure;
  }

  A_NAME(new) = (char*)calloc(_BUFF, sizeof(char));
  A_PATTERN(new) = (char*)calloc(_BUFF, sizeof(char));
  A_HELP(new) = (char*)calloc(_BUFF, sizeof(char));

  if (!A_NAME(new) || !A_PATTERN(new) || !A_HELP(new))
  {
    #line __LINE__ __FILE__
    goto add_argument_failure;
  }

  strncpy(A_NAME(new), argname, _BUFF);
  strncpy(A_PATTERN(new), pattern, _BUFF);

  if (help)
    strncpy(A_HELP(new), help, _BUFF);

  A_TYPE(new) = type;
  A_NARGS(new) = nargs;
  A_PARAM(new) = NULL;
  
  if (type == MULTIPLE)
  {
    A_QUEUE(new) = queue_init(NULL, NULL, (free_proto)free, NULL); // storing dynamic char buffers -> classic free function
  }
  else
  {
    A_QUEUE(new) = NULL;
  }

  P_ARGS_AT(parser, P_CURRSZ(parser)++) = new;

  // if ((P_CURRSZ(parser)++) == P_MXSZ(parser))
  // {
  //   //TODO REALLOC
  // }

  return true;

  add_argument_failure:
    if (new)
    {
      if (A_NAME(new))
        free(A_NAME(new));
      if (A_HELP(new))
        free(A_HELP(new));
      if (A_PATTERN(new))
        free(A_PATTERN(new));
      if (A_QUEUE(new))
        queue_clean(A_QUEUE(new));
      free(new);
    }
    return false;
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
      {
        continue;
      }
      for (k = 1; k <= A_NARGS(arg); k++)
      {
        buff = (char*)calloc(_BUFF, sizeof(char));
        if (!buff)
        {
          #line __LINE__ __FILE__
          return false;
        }
        strncpy(buff, argv[i+k], _BUFF);
        if (A_TYPE(arg) == SINGLE)
        {
          A_PARAM(arg) = buff;
        }
        else
        {
          queue_insert(A_QUEUE(arg), buff); // trust this won't fail
        }
      }
      i += (k - 1);
      break; // argument with same pattern won't be read nor stored
    }
  }

  return true;
}

char *argparse_get_arg(Parser *p, const char *argname)
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
      if (A_TYPE(P_ARGS_AT(p, i)) != SINGLE)
        return NULL;
      return A_PARAM(P_ARGS_AT(p, i));
    }
  }

  return NULL;
}

queue_t *argparse_get_args(Parser *p, const char* argname)
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
      if (A_TYPE(P_ARGS_AT(p, i)) == MULTIPLE)
        return A_QUEUE(P_ARGS_AT(p, i));
      return NULL;
    }
  }

  return NULL;
}

bool argparse_is_present(Parser *p, const char *argname)
{
  size_t i;
  
  if (!p || !argname)
  {
    #line __LINE__ __FILE__
    return false;
  }

  for (i = 0; i < P_CURRSZ(p); i++)
  {
    if (!strncmp(A_NAME(P_ARGS_AT(p, i)), argname, _BUFF))
    {
      return true;
    }
  }

  return false;
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
        if (A_NAME(P_ARGS_AT(p, i)) != NULL)
          free(A_NAME(P_ARGS_AT(p, i)));
        if (A_HELP(P_ARGS_AT(p, i)) != NULL)
          free(A_HELP(P_ARGS_AT(p, i)));
        if (A_PATTERN(P_ARGS_AT(p, i)) != NULL)
          free(A_PATTERN(P_ARGS_AT(p, i)));
        if (A_PARAM(P_ARGS_AT(p, i)) != NULL)
          free(A_PARAM(P_ARGS_AT(p, i)));
        if (A_QUEUE(P_ARGS_AT(p, i)) != NULL)
          queue_clean(A_QUEUE(P_ARGS_AT(p, i)));
        free(P_ARGS_AT(p, i));
      }
    }
    
    free(P_ARGS(p));
  }

  if (P_DESCRP(p) != NULL)
  {
    free(P_DESCRP(p));
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
    
    switch (A_TYPE(P_ARGS_AT(p, i)))
    {
    case EMPTY:
      TO_FILE(out, "\t\t\t - @@ EMPTY ARGUMENT @@\n");
      break;
    case SINGLE:
      TO_FILE(out, "\t\t\t - %s\n", A_PARAM(P_ARGS_AT(p, i)));
      break;
    default:
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
}
#else
void argparse_print_args(Parser *p, FILE *out){}
#endif