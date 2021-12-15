/**
 * @file power.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdint.h>

#include "util.h"
#include "gmp.h"

#ifndef _POWER_H_
#define _POWER_H_

typedef enum _def_err_code mpz_power_error_t;

typedef struct _power_t power_t;

power_t* power_init();

mpz_power_error_t power_setup(power_t *data, const char *b, const char *p, const char *m);

mpz_power_error_t power_setup_mpz(power_t *data, mpz_t b, mpz_t p, mpz_t m);

mpz_power_error_t power_free(power_t *data);

mpz_power_error_t power_compute(power_t *data, mpz_t result);

#endif