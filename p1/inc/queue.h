/**
 * Queue Module
 * 
 * @ @ @ @ @ @
 *
 * Queue for using FIFO operations
 * It supports basic operations
 * as insert and extract
 *
 * We're looking forward future implementations
 * that could support iterators and some other things,
 * but for now just:
 *
 *    -> create()
 *    -> insert()
 *    -> extract()
 *    -> size()
 *    -> isFull()
 *    -> isEmpty()
 *    -> destroy()
 *
 * Remember that this queue, for now, it can
 * be expanded where size limits are reached and it's
 * done dynamically, but currently we do not give support
 * to decrease queue size. It'll be implemented soon.
 *
 * @ @ @ @ @ @
 *
 * @version 1.0
 * @date 2021 / 03 / 29
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Compare function prototype
 * @param void first element
 * @param void second element
 * @return 0 if equals, else difference
 */
typedef int (*cmp_proto)(void*, void*);

/**
 * Copy function prototype
 * @param void element to copy
 * @return void copy of the element
 */
typedef void* (*cpy_proto)(void*);

/**
 * Free function prototype
 * @param void element to be removed
 */
typedef void (*free_proto)(void*);

/**
 * Print function prototype
 * @param FILE stream where to print
 * @param void element to print
 */
typedef void (*print_proto)(FILE*, void*);

/**
 * Type definition for queue
 */
typedef struct _queue_t queue_t;

/**
 * Creates new queue
 *
 * If prototypes are passed they can be used later (e.g pass free_proto for cleaning
 * queue and it's nodes)
 *
 * @param cmp_proto for comparing two values, currently unused
 * @param cpy_proto for copying value, currently unused
 * @param free_proto for cleaning values inside queue instead of perform loop of extractions
 * @param print_proto for printing queue info, currently unused
 * @return queue pointer
 */
queue_t *queue_init(cmp_proto, cpy_proto, free_proto, print_proto);

/**
 * Inserts info in supplied queue
 *
 * @param queue where to insert data
 * @param void* info
 * @return if operation could be performed
 */
bool queue_insert(queue_t *, void*);

/**
 * Inserts info in supplied queue
 *
 * @param queue where from data'll be retrieved
 * @return first info in queue (FIFO operation)
 */
void *queue_extract(queue_t *);

/**
 * Tells queue size
 *
 * @param queue to check
 * @return queue size
 */
uint_fast64_t queue_size(queue_t *);

/**
 * Tells queue maximum size
 *
 * @param queue to check
 * @return queue max size
 */
uint_fast64_t queue_max_size(queue_t *);

/**
 * Tells queue factor (current size / maximum size)
 *
 * @param queue to analyze
 * @return queue load factor
 */
float queue_load_factor(queue_t *queue);

/**
 * Tells if queue is full
 *
 * @param queue to check
 * @return if queue is full
 */
bool queue_isFull(queue_t *);

/**
 * Tells if queue is empty
 *
 * @param queue to check
 * @return if queue is empty
 */
bool queue_isEmpty(queue_t *);

/**
 * Cleans queue and all it's data if free_proto was supplied
 *
 * @param queue to be cleaned
 */
void queue_clean(queue_t *);

#endif
