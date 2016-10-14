
#include <stdlib.h>

#include "memory.h"

/*
 * Types internal
 */

typedef struct mem_node{

  void* loc;
  size_t size;

  mem_node* next;
}mem_node;

typedef struct buffer{

  size_t free;

  mem_node* record;
  mem_node* empty;
  char* buff;
} buffer;

typedef struct unmanaged_buff{

  int free;

  mem_node* buff;
} unmanaged_buff;

/*
 * File scope variables
 */

static unsigned int num_buff = 0;
static buffer** memory = NULL;
static unsigned int num_mgr = 0;
static unmanaged_buff** manager = NULL;
static mem_node* recycle = NULL;

/*
 * Invariance
 */

static void node_invariant( mem_node* node, int* x ){
#ifndef NDEBUG
  if( node != NULL ){

    assert( node->loc );
    assert( node->size > 0 && node->size < INIT_SIZE );
    *x += node->size;
    node_invariant( node->next, x )
  }
#endif//NDEBUG
}

static void manager_invariant( buffer* manager ){
#ifndef NDEBUG
  assert( manager );
  assert( manager->record || manager->empty );
  assert( manager->buff == manager );
  int x = 0;
  node_invariant( manager->record, &x );
  node_invariant( manager->empty, &x );
  assert( x < INIT_SIZE );

#endif//NDEBUG
}

/*
 * File scope functions
 */

static mem_node** tail( mem_node** n ){

  mem_node** ret = n;
  while( *ret != NULL ){
    ret = &ret->next;
  }

  return ret;
}

static mem_node* alloc_node(){

  mem_node* ret = NULL;
  unmanaged_buff *curr = NULL;

  if( recycle != NULL ){

    ret = recycle;
    recycle = ret->next;
    ret->next = NULL;
    return ret;
  }

  int i;
  for( i=0; i<num_buff && ret == NULL; ++i ){

    curr = manager[i];
    if( curr->free > 0 ){

      ret = curr->buff[INIT_SIZE/4 - curr->free];
    }
  }

  if( ret == NULL ){

    manager = realloc( manager, num_mgr+1 * sizeof( unmanaged_buff* ) );
    if( !manager ){ exit(1); }
    curr = malloc( sizeof( unamanged_buff ) +
                   INIT_SIZE/4 * sizeof( mem_node ) );
    if( !curr ){ exit(1); }

    manager[num_mgr] = curr;

    curr->open = INIT_SIZE/4;
    curr->recycle = NULL;
    curr->buff = &curr[1];

    ++num_mgr;
  }

  curr->open -= 1;
  return ret;
}

/*
 * Public scope
 */

void* alloc( size_t bytes ){

  assert( bytes > 0 );
  assert( bytes < INIT_SIZE - sizeof( buffer ) /*increase INIT_SIZE*/);

  void* ret = NULL;

  buffer* curr;
  mem_node* node = NULL;
  int i;
  for( i=0; i<num_buff && ret == NULL; ++i ){

    curr = memory[i];

    if( curr->free > bytes ){

      node = curr->empty;
      prev = NULL;
      while( node != NULL ){

        if( node->size == bytes ){

          ret = node->loc;

          if( prev == NULL ){

            curr->empty = node->next;
          }else{

            prev->next = node->next;
          }

          node->next = curr->record;//PUSH
          curr->record = node;

          return ret;
        }else if( node->next == NULL && node->size > bytes ){

          ret = node->loc;
          break;
        }else{

          prev = node;
          node = node->next;
        }
      }
    }
  }

  if( ret == NULL ){

    memory = realloc( memory, num_buff+1 * sizeof( buffer* ) );
    if( !memory ){ exit(1); }
    curr = malloc( INIT_SIZE );
    if( !curr ){ exit(1); }

    memory[num_buff] = curr;

    node = alloc_node();
    node->loc = &curr[1];
    node->size = INIT_SIZE - sizeof( buffer );
    node->next = NULL;

    curr->empty = node;

    ret = node->loc;

    ++num_buff;
  }

  prev = node;
  node = alloc_node();

  node->loc = prev->loc;
  node->size = bytes;
  node->next = curr->record; //PUSH
  curr->record = node;

  prev->size -= bytes;
  prev->loc += bytes;

  return ret;
}

void drop( void* loc ){

  assert( loc );

  buffer* curr;
  mem_node* node = NULL;


  int i;
  for( i=0; i<num_buff; ++i ){

    curr = memory[i];

    if( curr + INIT_SIZE > loc && loc > curr ){

      node = curr->record;
      while( node!=NULL ){

        if( node->loc == loc ){

          if( prev == NULL ){
            curr->record = node->next;
          }else{
            prev->next = node->next;
          }

          node->next = curr->empty;//PUSH
          curr->empty = node;
          return;
        }
        prev = node;
        node = node->next;
      }
    }
  }

  assert( false /*No record of that pointer being allocated
                 *You're either trying to drop a dropped pointer
                 *or fed an invalid pointer*/);
}

void flush_memory(){

  int i;

  for( i=0; i<num_buff; ++i ){

    free( memory[i] );
  }
  for( i=0; i<num_mgr; ++i ){

    free( manager[i] );
  }
}
