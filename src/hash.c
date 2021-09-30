/**
 * 
 *  HEADER
 * 
 * 
 */

#include "hash.h"

#define H__MAX( h ) ( h )->max_size
#define H__SIZE( h ) ( h )->curr_size
#define H__FACTOR( h ) ( h )->factor 
#define H__NODES( h ) ( h )->nodes
#define H__NODE_AT( h, i ) ( h )->nodes[i]
#define H__INFO( h, i ) ( h )->nodes[i]->info

#define NS__INFO_AT( n, i ) ( n[i] )->info
#define N__INFO( n ) ( n )->info

typedef struct _probing_response_t probing_response_t;
typedef struct _node node_t;

struct _probing_response_t
{
  bool present;
  uint_fast64_t index;
};

/**
 * node_t in hash
 */
struct _node {
  void *info; /* node_t info */
};

/**
 * Main hash structure
 */
struct _hash_t {
  uint_fast64_t max_size; // Max available size
  uint_fast64_t curr_size; // Current size
  float factor; // Load factor
  node_t **nodes; /* hash_t nodes which store info */
  hashcode_t hashcode; // function that
  equals_t equals;
  clean_t clean;
};

/* ----------------------- */

/* hash FUNCTIONS DECLARATIONS */

/**
 * Starts hash nodes array
 * @param hash where nodes will be stored
 * @return if was posible
 */
static bool init_nodes( hash_t *hash );

/**
 * Checks if node is empty
 * @param node to check
 * @return if it's empty
 */
static bool node_isEmpty( node_t *node );

/**
 * Initializes a new node
 * @param node to initialize
 * @param info to store in node
 * @return same node
 */
static node_t* init_node( void* info );

static void clean_nodes( node_t **nodes,clean_t clean, uint_fast64_t max_size );

/**
 * Linear probing for hash structure
 * @param hash to check
 * @param info info to check
 * @return probing_response_t object
 */
static probing_response_t linearProbing( 
  node_t **nodes,
  void *info,
  hashcode_t hashcode,
  equals_t equals,
  uint_fast64_t max_size
 );

/**
 * Refactors hash size if it's needed
 * @param hash to resize
 */
static void refactor_ifNeeded( hash_t *hash );

/* ----------------------- */

hash_t *hash_init( hashcode_t hashcode, equals_t equals, clean_t clean )
{
  hash_t *hash;

  if( !hashcode || !equals || !clean )
    return NULL;

  hash = ( hash_t* )calloc( 1,sizeof( hash_t ) );
  if( !hash )
    return NULL;
  
  hash->hashcode = hashcode;
  hash->equals = equals;
  hash->clean = clean;

  if( !init_nodes( hash ) )
  {
    hash_clean( hash );
    return NULL;
  }

  return hash;
}

static bool init_nodes( hash_t *hash )
{
  if( !hash )
    return false;

  H__NODES( hash ) = (node_t** )calloc(_DEF_HASHLEN_, sizeof(node_t* ) );
  H__MAX( hash ) = _DEF_HASHLEN_;
  H__SIZE( hash ) = 0;
  H__FACTOR( hash ) = 0;

  memset(H__NODES( hash ), 0, sizeof(node_t* )*_DEF_HASHLEN_ );

  if(!H__NODES( hash ) )
  {
    hash_clean(hash );
    return false;
  }

  return true;
}

void hash_clean(hash_t *hash )
{  
  if(!hash )
    return;

  clean_nodes(H__NODES( hash ), hash->clean, H__MAX( hash ) );

  free(H__NODES( hash ) );

  free(hash );
  
}

bool hash_encode(hash_t *hash, void* info )
{
  probing_response_t response;
  
  if(!info )
    return false;

  response = linearProbing(
    H__NODES( hash ),
    info,
    hash->hashcode,
    hash->equals,
    H__MAX( hash )
 );

  if( response.present == true )
    return false;

  H__NODE_AT( hash, response.index ) = init_node(info );
  H__SIZE( hash ) += 1;

  refactor_ifNeeded( hash );
  
  return true;
}

void* hash_decode( hash_t *hash, void* info )
{
  probing_response_t response;

  response = linearProbing( 
    H__NODES( hash ),
    info,
    hash->hashcode,
    hash->equals,
    H__MAX( hash )
 );

  
  if( response.present == false )
    return NULL;

  return H__INFO( hash, response.index );
}

static probing_response_t linearProbing( 
  node_t **nodes,
  void* info,
  hashcode_t hashcode,
  equals_t equals,
  uint_fast64_t max_size
 )
{
  probing_response_t response;
  uint_fast64_t hashed, val;
  uint_fast64_t i;
  
  hashed = hashcode( info ) % max_size;

  for ( i = 0; i < max_size; i++ )
  {
    val = ( hashed + i )%max_size;


    if( !node_isEmpty( nodes[val] ) )
    {
      if( equals( NS__INFO_AT( nodes, val ), info ) )
      {
        response.index = val;
        response.present = true;
        return response;
      }
    }
    else
    {
      response.index = val;
      response.present = false;
      return response;
    }
  }

  response.present = false;
  response.index = max_size;
  
  return response;
}

bool hash_contains( hash_t *hash, void* info )
{
  probing_response_t response;

  if( !hash || !info )
    return false;

  if( H__FACTOR( hash ) == 0 )
    return false;

  response = linearProbing( 
    H__NODES( hash ),
    info,
    hash->hashcode,
    hash->equals,
    H__MAX( hash )
 );

  return response.present;
}

static void refactor_ifNeeded( hash_t *hash )
{
  node_t **__nodes;
  probing_response_t r_response;
  uint_fast64_t __size;
  uint_fast64_t i;
  
  if( !hash )
    return;
  
  H__FACTOR( hash ) = ( float )( H__SIZE( hash ) )/( float )( H__MAX( hash ) );

  if( H__FACTOR( hash ) < _HIGH_CRITICAL_FACTOR_ )
    return;

  __size = ( H__MAX( hash ) ) << 1;
  __nodes = ( node_t** )calloc( __size, sizeof( node_t* ) );
  memset( __nodes, 0, sizeof( node_t* )*_DEF_HASHLEN_ );

  
  for( i = 0; i < H__MAX( hash ); i++ )
  {
    if( node_isEmpty( H__NODE_AT( hash, i ) ) )
    {
      free( H__NODE_AT( hash, i ) );
      continue;
    }
    
    r_response = linearProbing( 
      __nodes,
      H__INFO( hash, i ),
      hash->hashcode,
      hash->equals,
      __size
    );

    __nodes[r_response.index] = H__NODE_AT( hash, i );
  }
  
  free( H__NODES( hash ) );
  H__NODES( hash ) = __nodes;
  H__MAX( hash ) = __size;
  H__FACTOR( hash ) = ( float )H__SIZE( hash )/( float )H__MAX( hash );

}

static node_t* init_node( void* info )
{
  node_t *node;
  
  if( !info )
    return NULL;
  
  node = ( node_t* )calloc( 1,sizeof( node_t ) );
  N__INFO( node ) = info; 

  return node;
}

static void clean_nodes( node_t **nodes, clean_t clean, uint_fast64_t max_size )
{
  uint_fast64_t i;
  
  for( i = 0; i < max_size; i++ )
  {
    if( node_isEmpty( nodes[i] ) )
      continue;
    if( NS__INFO_AT( nodes, i ) != NULL )
    {
      clean( NS__INFO_AT( nodes, i ) );
    }
    free( nodes[i] );
    nodes[i] = NULL;
  }
}

static bool node_isEmpty( node_t *node )
{ 
  return node == NULL;
}