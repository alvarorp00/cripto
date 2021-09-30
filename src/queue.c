/**
 * Queue
 * @version 0.1
 */

#include "queue.h"

/**
 * Queue initial size
 */
#define INITIAL_SIZE 8

/**
 * Quick access definitions. Easy renaming
 */
#define Q_NODES(q) (q)->nodes
#define Q_NODE_AT(q, i) (q)->nodes[i]
#define Q_REAR(q) (q)->rear
#define Q_FRONT(q) (q)->front
#define Q_CURRSIZE(q) (q)->currSize
#define Q_MAXSIZE(q) (q)->maxSize
#define Q_CMP(q) (q)->cmp_proto
#define Q_CPY(q) (q)->cpy_proto
#define Q_FREE(q) (q)->free_proto
#define Q_PRINT(q) (q)->print_proto
#define NODE_INFO(n) (n)->info

/**
 * In resize queue must be iterated and
 * nodes must be reorganized to supply
 * trustworthy info, bcs if we resize and
 * updated values but queue is in an inconsistent 
 * state (e.g. as: [3, 4, X, X, X, 0, 1, 2 ] ),
 * if we resize it and add new values it'll be
 * inconsistent (showing last example: [3, 4, 5, 6, 7, 8, 1, 2, X, X, X, X, X, X] ),
 * so we must reorder it to avoid this problems
 */

/**
 * Control minimum load size in queue
 * If queue load factor is under this,
 * it must decrease it's maximum size
 */
#define __LOW_CRITICAL_FACTOR 0.2

/**
 * Control maximum load size in queue
 * If queue load factor is above this,
 * it must increase it's maximum size
 */
#define __HIGH_CRITICAL_FACTOR 0.85

/**
 * Structure that is stored
 * inside queue, instead of using
 * an array of (void**) inside queue
 */
typedef struct{
  void *info; /* info field of queue node */
}queue_node_t;

/**
 * Queue structure
 * with each of it's fields
 */
struct _queue_t {
  int_fast64_t rear; /* points towards it's last element */
  uint_fast64_t front; /* points towards it's first element */
  uint_fast64_t maxSize; /* maximum size of the queue */
  uint_fast64_t currSize; /* current load of the queue */
  queue_node_t **nodes; /* nodes in queue, whose info field stores info supplied */
  cmp_proto cmp_proto; /* function to compare info, currently not used */
  cpy_proto cpy_proto; /* function to cpy info, currently not used */
  free_proto free_proto; /* function to clean info, used in queue_destroy() */
  print_proto print_proto; /* function to print info, currently not used */
};

/* # # # # ! STATIC ! # # # # */

/**
 * Resizes queue to provided size. It
 * fills nodes with previous info.
 */
static bool resize(queue_t *queue, uint_fast64_t _size);

/**
 * Increases queue maximum size
 * by doing a realloc of **nodes array
 *
 * @param queue to be expanded
 * @return if operation could have been performed
 */
static bool expand(queue_t *queue);

/**
 * Decreases queue maximum size
 * by doing a realloc on **nodes array
 *
 * @param queue that will be shrunk
 * @return if operation could have been performed
 */
static bool shrink(queue_t *queue);

/**
 * Creates queue_node_t at position with supplied info
 *
 * @param queue where node is stored
 * @param idx of the node
 * @param info of the node
 */
static bool node_create(queue_t *queue, uint_fast64_t idx, void *info);

/**
 * Deletes queue_node_t from queue with specific index
 *
 * @param queue where node is stored
 * @param idx of the node
 * @param clean_info indicates if node's info must be deleted or not
 */
static bool node_clean(queue_t *queue, uint_fast64_t idx, bool clean_info);

/* - - - - ! METHODS ! - - - - */

queue_t *queue_init(
  cmp_proto cmp_fn,
  cpy_proto cpy_fn,
  free_proto free_fn,
  print_proto print_fn
)
{
  queue_t *queue;
  
  size_t i;

  queue = (queue_t*)malloc(sizeof(queue_t));
  if (!queue)
  {
    return NULL;
  }

  Q_NODES(queue) = (queue_node_t**)calloc(INITIAL_SIZE, sizeof(queue_node_t*));

  if(!Q_NODES(queue))
  {
    queue_clean(queue);
    return NULL;
  }

  Q_CMP(queue) = cmp_fn != NULL ? cmp_fn : NULL;
  Q_CPY(queue) = cpy_fn != NULL ? cpy_fn : NULL;
  Q_FREE(queue) = free_fn != NULL ? free_fn : NULL;
  Q_PRINT(queue) = print_fn != NULL ? print_fn : NULL;

  Q_CURRSIZE(queue) = 0;
  Q_MAXSIZE(queue) = INITIAL_SIZE;
  Q_FRONT(queue) = 0;
  Q_REAR(queue) = -1;
  
  return queue;

}

bool flag = false;

bool queue_insert(queue_t *queue, void* info)
{
  uint_fast64_t _prevsize;
  float factor;
  
  if (!queue || !info)
  {
    return false;
  }

  if ((Q_CURRSIZE(queue)) == Q_MAXSIZE(queue))
  {
    return false;
  }

  ++Q_REAR(queue);

  if (Q_REAR(queue) == Q_MAXSIZE(queue))
  {
    Q_REAR(queue) = 0; // better than performing % for each insert
  }
  
  if (!node_create(queue, Q_REAR(queue), info))
  {
    --Q_REAR(queue); // reset rear, it was not inserted
    if (Q_REAR(queue) == -1)
    {
      Q_REAR(queue) = Q_MAXSIZE(queue) - 1; // important
    }
    perror("Error allocating queue node memory");
    return false;
  }

  factor = ((float)Q_CURRSIZE(queue) / Q_MAXSIZE(queue));

  if (factor > __HIGH_CRITICAL_FACTOR)
  {
    expand(queue);
  }

  return true;

}

void *queue_extract(queue_t *queue)
{
  void *info = NULL;
  float factor;
  
  if(!queue || queue_isEmpty(queue) || !Q_NODE_AT(queue, Q_FRONT(queue)))
  {
    return NULL;
  }

  info = NODE_INFO(Q_NODE_AT(queue, Q_FRONT(queue)));

  node_clean(queue, Q_FRONT(queue), false);

  Q_FRONT(queue)++;

  if(Q_FRONT(queue) == Q_MAXSIZE(queue))
  {
    Q_FRONT(queue) = 0;
  }

  factor = ((float)Q_CURRSIZE(queue) / Q_MAXSIZE(queue));

  if (factor < __LOW_CRITICAL_FACTOR)
  {
    shrink(queue);
  }

  return info;
}

uint_fast64_t queue_size(queue_t *queue)
{
  return queue == NULL ? 0 : Q_CURRSIZE(queue);
}

uint_fast64_t queue_max_size(queue_t *queue)
{
  return queue == NULL ? 0 : Q_MAXSIZE(queue);
}

bool queue_isFull(queue_t *queue)
{
  return queue == NULL ? false : Q_CURRSIZE(queue) == Q_MAXSIZE(queue);
}

bool queue_isEmpty(queue_t *queue)
{
  return queue == NULL ? true : Q_CURRSIZE(queue) == 0;
}

void queue_clean(queue_t *queue)
{
  uint_fast64_t i, j;


  if (queue == NULL)
  {
    return;
  }

  if(Q_NODES(queue) != NULL)
  {
    if(Q_FREE(queue) != NULL)
    {
      for (i = Q_FRONT(queue); ((i + 1) % Q_MAXSIZE(queue)) != Q_FRONT(queue); i++) // best performance, less accesses
      {
        j = (i % Q_MAXSIZE(queue));
        if (Q_NODE_AT(queue, j) == NULL)
        {
          continue;
        }
        if (NODE_INFO(Q_NODE_AT(queue, j)) != NULL) // take care about repeated elements, bcs they will be free'd twice as no cmp function is supported
        {
          Q_FREE(queue)(NODE_INFO(Q_NODE_AT(queue, j)));
          NODE_INFO(Q_NODE_AT(queue, j)) = NULL;
        }
        free(Q_NODE_AT(queue, j));
        Q_NODE_AT(queue, j) = NULL;
      }
    }
    else
    {
      for (i = Q_FRONT(queue); ((i + 1) % Q_MAXSIZE(queue)) != Q_FRONT(queue); i++) // best performance, less accesses
      {
        j = (i % Q_MAXSIZE(queue));
        if (Q_NODE_AT(queue, j) == NULL)
        {
          continue;
        }
        free(Q_NODE_AT(queue, j));
        Q_NODE_AT(queue, j) = NULL;
      }
    }
    
    free(Q_NODES(queue));
    Q_NODES(queue) = NULL;
  }

  free(queue);
  queue = NULL;
}

float queue_load_factor(queue_t *queue)
{
  return (float)((queue == NULL) ? -1 : ((float)Q_CURRSIZE(queue) / Q_MAXSIZE(queue)));
}

/* LOCAL METHODS */

static bool expand(queue_t *queue)
{
  return resize(queue, Q_MAXSIZE(queue) << 1);
}

static bool shrink(queue_t *queue)
{
  return resize(queue, Q_MAXSIZE(queue) >> 1);
}

static bool resize(queue_t *queue, uint_fast64_t _size)
{
  queue_node_t **__nodes;
  uint_fast64_t _front;
  int_fast64_t _rear;

  uint_fast64_t i;
  
  if (!queue)
  {
    return false;
  }

  if (_size < INITIAL_SIZE)
  {
    return false;  // don't make smaller than start size
  }

  __nodes = NULL;
  _front = 0;
  _rear = -1;

  __nodes = (queue_node_t**)calloc(_size, sizeof(queue_node_t*));

  if (!__nodes)
  {
    return false; // can't perform operation
  }

  for (i = Q_FRONT(queue); ((i + 1) % Q_MAXSIZE(queue)) != Q_FRONT(queue) && Q_NODE_AT(queue, i % Q_MAXSIZE(queue)) != NULL; i++)
  { 
    __nodes[++_rear] = Q_NODE_AT(queue, (i % Q_MAXSIZE(queue))); // copy nodes
  }

  Q_MAXSIZE(queue) = _size;
  Q_FRONT(queue) = _front;
  Q_REAR(queue) = _rear;

  free(Q_NODES(queue)); // clean ** node array, now we have it resized !

  Q_NODES(queue) = __nodes;

  return true;
}

static bool node_create(queue_t *queue, uint_fast64_t idx, void *info)
{
  if (!queue)
  {
    return false;
  }

  if (Q_NODE_AT(queue, idx) != NULL || idx > Q_MAXSIZE(queue))
  {
    perror("Can't create node");
    return false; // can't perform operation
  }

  Q_NODE_AT(queue, idx) = (queue_node_t*)malloc(sizeof(queue_node_t));

  if (!Q_NODE_AT(queue, idx))
  {
    perror("Can't allocate queue node memory");
    return false;
  }

  NODE_INFO(Q_NODE_AT(queue, idx)) = info;

  (Q_CURRSIZE(queue))++;

  return true;

}

/* Queue node operators */

static bool node_clean(queue_t *queue, uint_fast64_t idx, bool clean_info)
{
  if (!queue)
  {
    return false;
  }

  if (!Q_NODE_AT(queue, idx) || idx > Q_MAXSIZE(queue))
  {
    return false;
  }

  if (clean_info && Q_FREE(queue) != NULL && NODE_INFO(Q_NODE_AT(queue, idx)) != NULL)
  {
    Q_FREE(queue)(NODE_INFO(Q_NODE_AT(queue, idx)));
  }

  NODE_INFO(Q_NODE_AT(queue, idx)) = NULL;

  free(Q_NODE_AT(queue, idx));

  Q_NODE_AT(queue, idx) = NULL;

  (Q_CURRSIZE(queue))--;

  return true;
}
