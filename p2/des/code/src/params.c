#include "logger.h"
#include "params.h"

#include <argp.h>

#define LOG_COLORS

static des_params_t g_des_params;

#define STR_P( p ) \
  "" __LOG_YELLOW "-" p __LOG_RS ""

#define DEFAULT_OPTIONS \
  { "encrypt",    'C', 0,             0,                      "Encrypt text"}, \
  { "decrypt",    'D', 0,             0,                      "Decrypt text"}, \
  { "input",      'i', "PATH",        0,                      "Input file. Default stdin"}, \
  { "output",     'o', "PATH",        0,                      "Output file. Default stdout"}, \
  { "verbose",    'v', "NUMBER",      OPTION_ARG_OPTIONAL,    "Enables verbose output, optionally the logging level can be supplied"}

#define OPTIONS_END {0}

static struct argp_option des_options[] = {
  DEFAULT_OPTIONS,
  { "iv",        't',  "HEXSTR",       0,             "Initialization Vector"},
  { "key",       'k',  "HEXSTR",       0,             "key of 64b[56b + 8parity]"},
  { "sbit",      'S',  "NUMBER",       0,             "Number of bits to shift [1,8,16,32,64]"},
  OPTIONS_END
};

error_t _def_parse_opt( int key, char *arg, def_params_t *arguments ) {

  switch ( key ) {

    case 'v':
      
      if ( arg ) {
        int l = atoi( arg );
        if ( l > 0 ) {
          logger_log_level( l );
        } else {
          LOG_ERR("invalid logging level %s%d%s\n", __LOG_YELLOW, l, __LOG_RS );
          return ARGP_KEY_ERROR;
        }

      } else {
        logger_log_level( 5 );
      }

      break;
    
    case 'D':
      arguments->D = true;
      break;

    case 'C':
      arguments->C = true;
      break;

    case 'i':
      arguments->infPath = arg;
      break;

    case 'o':
      arguments->outfPath = arg;
      break;
  }

  return 0;

}

error_t _des_parse_opt( int key, char *arg, struct argp_state *state ) {
  
  des_params_t *arguments = state->input;
  
  error_t ret = _def_parse_opt( key, arg, &arguments->def );
  
  if ( ret ) return ret;

  switch (key)
  {
  case 'k':
    arguments->key  = arg;
    break;
  case 't':
    arguments->iv   = arg;
  case 'S':
    arguments->sbit = (byte)atoi(arg);
  default:
    break;
  }

  return 0;
}
static const char doc[] = "";
static const char args_doc[] = "";

static struct argp des_argp = { des_options, _des_parse_opt, args_doc, doc };

error_t _check_defaults( def_params_t *def ) {
  
  error_t ret = 0;

  if ( def->D && def->C ) {
    LOG_ERR("%s and %s can not be used simultaneously\n", STR_P( "C" ), STR_P( "D" ) );
    ret = ARGP_KEY_ERROR;  
  }
  if ( !def->D && !def->C ) {
    LOG_ERR("action not specified (%s|%s)\n", STR_P( "C" ), STR_P( "D" ) );
    ret = ARGP_KEY_ERROR;  
  }

  return ret;
}

des_params_t* params_parse_des( int argc, char **argv ) {
  
  INIT_DEFAULT_PARAMS_STRUCT( g_des_params );

  g_des_params.key = NULL;
  g_des_params.iv = NULL;
  
  error_t ret = argp_parse( &des_argp, argc, argv, 0, 0, &g_des_params );

  ret += _check_defaults( &g_des_params.def );

  if ( g_des_params.iv == NULL ) {
    LOG_ERR("Initialization vector can't be void");
    ret = ARGP_KEY_ERROR;
  }

  if ( g_des_params.def.D && (g_des_params.key == NULL) ) {
    LOG_ERR("Key must be given for decipher process" )
    ret = ARGP_KEY_ERROR;
  }

  return ret ? NULL : &g_des_params;
}