#include "logger.h"
#include "params.h"

#include <argp.h>

#define LOG_COLORS

static power_params_t g_power_params;
static prime_params_t g_prime_params;

#define STR_P( p ) \
  "" __LOG_YELLOW "-" p __LOG_RS ""

#define DEFAULT_OPTIONS \
  { "output",     'o', "PATH",        0,                      "Output file. Required."}, \
  { "verbose",    'v', "NUMBER",      OPTION_ARG_OPTIONAL,    "Enables verbose output, optionally the logging level can be supplied"},\
  { "test",       't', 0,             0,                      "Program is run with test mode"}

#define OPTIONS_END {0}

static struct argp_option power_options[] = {
  DEFAULT_OPTIONS,
  { "base",        'b',  "NUMBER",       0,             "base to be powered"},
  { "pow",         'p',  "NUMBER",       0,             "power value"},
  { "modulus",     'm',  "NUMBER",       0,             "value used as modulus"},
  OPTIONS_END
};

static struct argp_option prime_options[] = {
  DEFAULT_OPTIONS,
  { "bits",        'b',  "NUMBER",       0,             "length of prime (in bits)"},
  { "sec",         'p',  "ERROR (prob)", 0,             "security"},
  { "test_req",    's',   0,             0,             "set this flag if prime displayed must succeed all tests run"},
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

    case 't':
      arguments->doTest = true;
    case 'o':
      arguments->outfPath = arg;
      break;
  }

  return 0;

}

error_t _power_parse_opt( int key, char *arg, struct argp_state *state ) {
  
  power_params_t *arguments = state->input;
  
  error_t ret = _def_parse_opt( key, arg, &arguments->def );
  
  if ( ret ) return ret;

  switch (key)
  {
  case 'b':
    arguments->base = arg;
    break;
  case 'p':
    arguments->pow = arg;
  case 'm':
    arguments->modulus = arg;
  default:
    break;
  }

  return 0;
}

error_t _prime_parse_opt( int key, char *arg, struct argp_state *state ) {
  
  prime_params_t *arguments = state->input;
  
  error_t ret = _def_parse_opt( key, arg, &arguments->def );
  
  if ( ret ) return ret;

  switch (key)
  {
  case 'b':
    arguments->bits = arg;
    break;
  case 'p':
    arguments->sec = arg;
    break;
  case 's':
    arguments->min_test_suc = true;
    break;
  default:
    break;
  }

  return 0;
}

static const char doc[] = "";
static const char args_doc[] = "";

static struct argp power_argp = { power_options, _power_parse_opt, args_doc, doc };
static struct argp prime_argp = { prime_options, _prime_parse_opt, args_doc, doc };

error_t _check_defaults( def_params_t *def ) {
  
  error_t ret = 0;

  // pass

  return ret;
}

power_params_t* params_parse_power( int argc, char **argv ) {
  
  INIT_DEFAULT_PARAMS_STRUCT( g_power_params );

  g_power_params.base    = NULL;
  g_power_params.pow     = NULL;
  g_power_params.modulus = NULL;

  error_t ret = argp_parse( &power_argp, argc, argv, 0, 0, &g_power_params );

  ret += _check_defaults( &g_power_params.def );

  if ( !g_power_params.def.doTest && g_power_params.base == NULL ) {
    LOG_ERR("No base given");
    ret = ARGP_KEY_ERROR;
  }

  if ( !g_power_params.def.doTest && g_power_params.pow == NULL ) {
    LOG_ERR("No power given");
    ret = ARGP_KEY_ERROR;
  }

  if ( !g_power_params.def.doTest && g_power_params.modulus == NULL ) {
    LOG_ERR("No modulus given");
    ret = ARGP_KEY_ERROR;
  }

  return ret ? NULL : &g_power_params;
}

prime_params_t* params_parse_prime( int argc, char **argv ) {
  
  INIT_DEFAULT_PARAMS_STRUCT( g_prime_params );

  g_prime_params.bits    = NULL;
  g_prime_params.sec     = NULL;
  g_prime_params.min_test_suc = false;
  
  error_t ret = argp_parse( &prime_argp, argc, argv, 0, 0, &g_prime_params );

  ret += _check_defaults( &g_prime_params.def );

  if ( !g_prime_params.def.doTest && g_prime_params.bits == NULL ) {
    LOG_ERR("No bits given");
    ret = ARGP_KEY_ERROR;
  }

  if ( !g_prime_params.def.doTest && g_prime_params.sec == NULL ) {
    LOG_ERR("No security given");
    ret = ARGP_KEY_ERROR;
  }

  return ret ? NULL : &g_prime_params;
}