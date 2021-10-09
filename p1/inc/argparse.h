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

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "queue.h"

typedef enum {
  EMPTY, SINGLE, MULTIPLE
}ArgType;

typedef struct _Parser Parser;

Parser* argparse_init();

bool argparse_add_argument(
  Parser *parser,
  const char *argname,
  ArgType type,
  const char *pattern,
  uint8_t nargs,
  const char *help
);

/**
 * @brief Currently stores arguments
 * as bytearray (char*). Future implementations
 * will add type options...
 * 
 * @param parser 
 * @param argc 
 * @param argv 
 * @return true 
 * @return false 
 */
bool argparse_parse_args(Parser *parser, int argc, const char *argv[]);

/**
 * @brief Returns queue
 * containing arguments
 * given to correspondant
 * param. If arg is EMPTY type,
 * it'll return a NULL pointer as
 * well as if argument option can't be found.
 * 
 * Also returns NULL when argument is of type SINGLE,
 * as they do not use a queue.
 * 
 * @param p 
 * @param argname 
 * @return queue_t* queue with all arguments given 
 */
queue_t* argparse_get_args(Parser *p, const char* argname);

/**
 * @brief Returns argument associated
 * with param. It just work's
 * with single param arguments.
 * If argument is not single type, it'll
 * return NULL
 * 
 * @param p parser
 * @param argname argument name
 * @return char* param
 */
char *argparse_get_arg(Parser *p, const char *argname);

/**
 * @brief tells if argument is present,
 * if it's been read
 * 
 * @param p parser
 * @param argname argument to be checked
 * @return true 
 * @return false 
 */
bool argparse_is_present(Parser *p, const char *argname);

/**
 * @brief Sets argparser description
 * 
 * @param parser 
 * @param descrp 
 * @return true 
 * @return false 
 */
bool argparse_set_descr(Parser *parser, const char *descrp);

/**
 * @brief Cleans parser,
 * all resources alloc'd by it
 * will be free'd
 * 
 * @param parser 
 */
void argparse_clean(Parser *parser);

/**
 * @brief Destructive function, as
 * it's purpose is for debugging and
 * just that!!
 * 
 * @param p parser
 * @param out where to put output
 */
void argparse_print_args(Parser *p, FILE *out);

#endif