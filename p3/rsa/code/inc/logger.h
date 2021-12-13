#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define LOG_COLORS
#define __VERSION "0.0.3"
#define STRN_SIZE 256


#define DO_FOREVER      while (1) {
#define END_FOREVER     }

#ifdef LOG_COLORS
  #define __LOG_RS "\033[0m"
  #define __LOG_RED "\033[0;31m"
  #define __LOG_BLUE "\033[0;34m"
  #define __LOG_YELLOW "\033[1;33m"
  #define __LOG_GREEN "\033[0;32m"
#else
  #define __LOG_RS ""
  #define __LOG_RED ""
  #define __LOG_BLUE ""
  #define __LOG_YELLOW ""
  #define __LOG_GREEN ""
#endif

#define __LOG_ERR_PREFIX "["__LOG_RED " ER " __LOG_RS"] "
#define __LOG_INFO_PREFIX "["__LOG_BLUE "INFO" __LOG_RS"] "
#define __LOG_OK_PREFIX "["__LOG_GREEN " OK " __LOG_RS"] "
#define __LOG_WARN_PREFIX "["__LOG_YELLOW "WARN" __LOG_RS"] "

#define __LOG( fp, lvl, str, ... ) \
  if ( lvl <= __log_level__ ) { \
    fprintf( fp, str, ##__VA_ARGS__ ); \
  }

/**
 * Use this macros to print at a certain log level
 */
#define LOGL_ERR( lvl, str, ... ) \
  __LOG( stderr, lvl, __LOG_ERR_PREFIX str, ##__VA_ARGS__ )

#define LOGL_OK( lvl, str, ... ) \
  __LOG( stdout, lvl, __LOG_OK_PREFIX str, ##__VA_ARGS__ )

#define LOGL_WARN( lvl, str, ... ) \
  __LOG( stdout, lvl, __LOG_WARN_PREFIX str, ##__VA_ARGS__ )

#define LOGL_INFO( lvl, str, ... ) \
  __LOG( stdout, lvl, __LOG_INFO_PREFIX str, ##__VA_ARGS__ )

/**
 * Use the following macros to ignore log level
 */
#define LOG_ERR( str, ... ) \
  LOGL_ERR( 0, str, ##__VA_ARGS__ );

#define LOG_OK( str, ... ) \
  LOGL_OK( 0, str, ##__VA_ARGS__ );

#define LOG_INFO( str, ... ) \
  LOGL_INFO( 0, str, ##__VA_ARGS__ );

#define LOG_WARN( str, ... ) \
  LOGL_WARN( 0, str, ##__VA_ARGS__ );

/**
 * @brief System call's logs
 */
#define LOG_SYS_ERR( fnstr, err, ... ) \
  LOGL_ERR( 5, fnstr"() : "__LOG_BLUE"%s"__LOG_RS" at %s:%d\n", ##__VA_ARGS__, strerror( err ), __FILE__, __LINE__ )

#define LOG_SYS_INFO( str, ... ) \
  LOGL_INFO( 5, str, ##__VA_ARGS__ )

#define LOG_SYS_WARN( str, ... ) \
  LOGL_WARN( 5, str, ##__VA_ARGS__ )

#define LOG_SYS_OK( str, ... ) \
  LOGL_OK( 5, str, ##__VA_ARGS__ )

/**
 * @brief Logs for child process
 */
#define __LOG_PID_PREFIX  "%d @ "


#define LOGL_P_ERR( lvl, str, ... ) \
  LOGL_ERR( lvl, __LOG_PID_PREFIX str, getpid(), ##__VA_ARGS__ )

#define LOGL_P_INFO( lvl, str, ... ) \
  LOGL_INFO( lvl, __LOG_PID_PREFIX str, getpid(), ##__VA_ARGS__ )

#define LOGL_P_OK( lvl, str, ... ) \
  LOGL_OK( lvl, __LOG_PID_PREFIX str, getpid(), ##__VA_ARGS__ )

#define LOGL_P_WARN( lvl, str, ... ) \
  LOGL_WARN( lvl, __LOG_PID_PREFIX str, getpid(), ##__VA_ARGS__ )


/**
 * @defgroup LOG_P_ Child process log
 * This macros should be used to log data from child process
 * @note This logs will be printed always as it's default level is `0`
 * @{
 */
#define LOG_P_ERR( str, ... ) \
  LOGL_P_ERR( 0, str, ##__VA_ARGS__ )

#define LOG_P_INFO( str, ... ) \
  LOGL_P_INFO( 0, str, ##__VA_ARGS__ )

#define LOG_P_OK( str, ... ) \
  LOGL_P_OK( 0, str, ##__VA_ARGS__ )

#define LOG_P_WARN( str, ... ) \
  LOGL_P_WARN( 0, str, ##__VA_ARGS__ )
/** @} */

/**
 * @defgroup LOG_PSYS System level child process log
 * This macros should be used to log system level data from child process
 * @note This logs will be printed for log levels `>= 5`
 * @{
 */
#define LOG_PSYS_ERR( fnstr, err, ... ) \
  LOGL_P_ERR( 5, fnstr"() : "__LOG_BLUE"%s"__LOG_RS" at %s:%d\n", ##__VA_ARGS__, strerror( err ), __FILE__, __LINE__ )

#define LOG_PSYS_WARN( fnstr, err, ... ) \
  LOGL_P_WARN( 5, fnstr"() : "__LOG_BLUE"%s"__LOG_RS" at %s:%d\n", ##__VA_ARGS__, strerror( err ), __FILE__, __LINE__ )

#define LOG_PSYS_INFO( str, ... ) \
  LOGL_P_INFO( 5, str, ##__VA_ARGS__ )

#define LOG_PSYS_OK( str, ... ) \
  LOGL_P_OK( 5, str, ##__VA_ARGS__ )
/** @} */

/**
 * @defgroup __BUFF Byte buffers
 * @{
 */

/** @brief Generic buffer */
#define __BUFF( name, size ) char name[ size ] = "";

/** @brief 16 B */
#define __16BUFF( name ) __BUFF( name, 16 )
/** @brief 32 B */
#define __32BUFF( name ) __BUFF( name, 32 )
/** @brief 64 B */
#define __64BUFF( name ) __BUFF( name, 64 )
/** @brief 128 B */
#define __128BUFF( name ) __BUFF( name, 128 )
/** @brief 256 B */
#define __256BUFF( name ) __BUFF( name, 256 )
/** @brief 1 KB */
#define __1KBUFF( name ) __BUFF( name, 1024 )
/** @brief 2 KB */
#define __2KBUFF( name ) __BUFF( name, 2048 )
/** @brief 4 KB */
#define __4KBUFF( name ) __BUFF( name, 4096 )
/** @brief 16 KB */
#define __16KBUFF( name ) __BUFF( name, 16384 )
/** @brief 32 KB */
#define __32KBUFF( name ) __BUFF( name, 32768 )
/** @brief 64 KB */
#define __64KBUFF( name ) __BUFF( name, 65536 )
/** @brief 128 KB */
#define __128KBUFF( name ) __BUFF( name, 131072 )

/** @} */

/**
 * @brief Frees a pointer if it's not null
 */
#define FREE_IF_NOT_NULL( ptr ) if ( (ptr) != NULL ) free( ptr );

void logger_filter_plain_text( char *src, int *len );
void logger_reduce_str( const char *filter, char *src, int *len );

int logger_random_btw( int a, int b );

unsigned int modulus( int a, int b );

/**
 * @brief This function is used to set a custom level for the log
 * 
 * @param level new log level 
 */
void logger_log_level( int level );


/**
 * @brief A global variable used to know the current log level
 * 
 * @note This variable should not be changed directly,
 * if you want to change the log level use logger_log_level() instead
 */
extern int __log_level__;

#endif
