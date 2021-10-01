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

#include "argparse.h"
#include "queue.h"

#define P_ARGS(p) (p)->args
#define P_ARGS_AT(p, i) (p)->args[i]
#define P_MXSZ(p) (p)->maxsize
#define P_CURRSZ(p) (p)->currsize
#define P_DESCRP(p) (p)->description

#define _INIT_PRSR_SZ 64
#define _BUFF 256

typedef struct _Argument Argument;

struct _Argument {
  char *argname;
  char *argpattern;
  uint8_t nargs;
  char *help;
  ArgType type;
};

struct _Parser {
  Argument **args;
  uint_fast64_t maxsize;
  uint_fast64_t currsize;
  char description[_BUFF*4];
};

void parser_init(Parser *parser)
{
  size_t i, j;
  
  if (!parser)
  {
    #line __LINE__ __FILE__
    return;
  }

  parser = (Parser*)malloc(sizeof(Parser));

  if (!parser)
  {
    #line __LINE__ __FILE__
    return;
  }

  P_MXSZ(parser) = _INIT_PRSR_SZ;
  P_CURRSZ(parser) = 0;

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
  uint8_t *nargs,
  char *help
)
{
  if (!parser || !argname)
  {
    #line __LINE__ __FILE__
    return false;
  }

  if (P_CURRSZ(parser) == P_MXSZ(parser))
  {
    #line __LINE__ __FILE__
    perror ("Not enough space inside argparser");
    return false;
  }


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
bool argparse_parse_args(Parser *parser, int argc, char *argv[])
{
  size_t i, j, k;
  
  if (!parser || !argv)
  {
    #line __LINE__ __FILE__
    return false;
  }

  return true;
}

void argparse_clean(Parser *p)
{
  size_t i, j, k;
  
  if (!p)
    return;

  // TODO

}