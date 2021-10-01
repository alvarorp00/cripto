/**
 * @file argparse.h
 * @author alvarorp00 (alvarorp00@sigsuspend.net)
 * @brief Simple module for parsing args
 * @version 0.1
 * @date 2021-09-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __ARGPARSE_H__
#define __ARGPARSE_H__

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  SINGLE, MULTIPLE
}ArgType;

typedef struct _Parser Parser;

void argparse_init(Parser *parser);

bool argparse_add_argument(
  Parser *parser,
  char *argname,
  ArgType type,
  char *pattern,
  uint8_t *nargs,
  char *help
);

bool argparse_parse_args(Parser *parser, int argc, char *argv[]);

bool argparse_set_descr(Parser *parser,  char *descrp);

void argparse_clean(Parser *parser);

#endif