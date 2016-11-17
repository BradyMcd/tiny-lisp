
/*MIT License
 * Copyright 2016, Brady McDonough <brady.mcd.codes@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <lval.h>
#include <memory.h>

/*
 *Internals
 */

struct lmgr{

  lval *reclaim;
  lval buffer[];
};

struct strmgr{
  unsigned int free;
  unsigned int next;
  char buff[];
};

static unsigned int strbuffs = 0;
static struct strmgr **strbuff = NULL;
static unsigned int lval_managers = 0;
static struct lmgr **lval_manager = NULL;

/**
 *List Manipulation
 */

lval *lval_pop( lval **node ){

  if( *node == NULL ){ return NULL; }
  lval *ret = *node;
  *node = (*node)->next;
  ret->next = NULL;
  return ret;
}
void lval_push( lval **node, lval *new ){

  assert( node != NULL );
  assert( new != NULL);
  new->next = *node;
  *node = new;
}

/**
 *Memory Operations
 */

lval *lalloc( ){

  lval *ret = NULL;
  struct lmgr *curr = NULL;
  unsigned int i;
  for( i = 0; i < lval_managers && ret == NULL; ++i ){

    curr = lval_manager[i];
    ret = lval_pop( &(curr->reclaim) );
  }
  if( ret == NULL ){

    //new manager
    lval_manager = realloc( lval_manager, sizeof( struct lmgr* ) *
                             ( lval_managers + 1 ) );
    curr = malloc( sizeof( struct lmgr ) +
                   sizeof( lval ) * INIT_SIZE );
    curr->reclaim = NULL;

    ret = &curr->buffer[0];
    ret->tag = LVAL_MGR;
    ret->num = INIT_SIZE;
    ret->next = NULL;
    lval_manager[lval_managers] = curr;
    ++ lval_managers;
  }

  assert( ret->tag == LVAL_MGR );
  if( ret->num > 1 ){
    lval *new_head = &ret[1];
    new_head->tag = LVAL_MGR;
    new_head->num = ret->num - 1;
    new_head->next = NULL;
    lval_push( &( curr->reclaim ), new_head );
  }

  ret->num = 0;
  ret->str = NULL;
  ret->asoc = NULL;
  ret->next = NULL;
  ret->fun = NULL;
  ret->tag = LVAL_NIL;
  return ret;
}

lval *stralloc( size_t bytes ){

  lval *ret = lalloc();
  unsigned int i;
  struct strmgr *curr = NULL;

  for( i = 0; i < strbuffs && ret->str == NULL; ++i ){

    curr = strbuff[i];
    if( ( ( INIT_CHAR - 1 ) - curr->next ) > bytes ){
      ret->str = &curr->buff[curr->next];
    }
  }

  if( ret->str == NULL ){

    strbuff = realloc( strbuff, sizeof( struct strmgr* )*( strbuffs + 1 ) );
    curr = malloc( sizeof( struct strmgr ) + INIT_CHAR );
    curr->free = 0;
    curr->next = 0;
    strbuff[strbuffs] = curr;
    strbuffs ++;
    ret->str = &curr->buff[curr->next];
  }

  assert( curr );
  assert( ret );
  assert( ret->str );

  ret->tag = LVAL_SYM;
  ret->num = bytes;

  curr->next += bytes;
  return ret;
}


void ldrop( lval *ptr ){

  assert( ptr );
  assert( ptr->tag != LVAL_MGR );

  if( ptr->next != NULL ){
    ldrop( ptr->next );
  }
  if( ptr->asoc != NULL ){
    ldrop( ptr->asoc );
  }

  unsigned int i;
  if( ptr->tag == LVAL_SYM || ptr->tag == LVAL_FN ||
      ptr->tag == LVAL_BUILTIN || ptr->tag == LVAL_ERR ){
    for( i = 0; i < strbuffs; ++i ){
      if( ( strbuff[i]->buff ) < ptr->str &&
          ( strbuff[i]->buff + INIT_CHAR ) > ptr->str ){
        strbuff[i]->free += ptr->num;

        if( strbuff[i]->free == strbuff[i]->next ){
          //space reclaim, very weak at the moment
          strbuff[i]->free = 0;
          strbuff[i]->next = 0;
        }
        break;
      }
    }
  }

  ptr->tag = LVAL_MGR;
  ptr->str = NULL;
  ptr->next = NULL;
  ptr->fun = NULL;
  ptr->asoc = NULL;
  ptr->num = 1;

  for( i = 0; i < lval_managers; ++i ){
    if( &lval_manager[i]->buffer[0] <= ptr &&
        &lval_manager[i]->buffer[INIT_SIZE - 1] > ptr ){
      lval_push( &lval_manager[i]->reclaim, ptr );
      return;
    }
  }

  assert( false /*Where did you get that pointer? Not from here*/);
}

void lval_cleanup(){

  unsigned int i;
  for( i = 0; i < lval_managers; ++i ){

    free( lval_manager[i] );
    lval_manager[i] = NULL;
  }
  free( lval_manager );
  lval_manager = NULL;
  lval_managers = 0;
}
