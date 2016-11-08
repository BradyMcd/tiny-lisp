
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

#include "memory.h"

/*
 *Internals
 */

struct lmgr{

  lval* reclaim;
};

struct strmgr{
  unsigned int free;
  unsigned int next;
  char* buff;
};

static unsigned int strbuffs = 0;
static struct strmgr** strbuff = NULL;
static unsigned int lval_managers = 0;
static struct lmgr** lval_manager = NULL;

/*
 *Public Interface
 */

/**
 *List Manipulation
 */

lval* lval_pop( lval** node ){

  if( *node == NULL ){ return NULL; }
  lval* ret = *node;
  *node = (*node)->next;
  ret->next = NULL;
  return ret;
}
void lval_push( lval** node, lval* new ){

  assert( node != NULL );
  assert( new != NULL);
  new->next = *node;
  *node = new;
}

/**
 *Memory Operations
 */

lval* lalloc( ){

  lval* ret;
  unsigned int i;
  for( i = 0; i < lval_managers && ret == NULL; ++i ){

    ret = lval_pop( &(lval_manager[i]->reclaim) );
  }
  if( ret == NULL ){

    //new manager
    lval_manager = realloc( lval_manager, sizeof( struct lmgr* ) *
                             ( lval_managers + 1 ) );
    lval_manager[i] = malloc( sizeof( lval ) * INIT_SIZE +
                              sizeof(struct lmgr ) );
    ret = (lval*)&lval_manager[i][1];
    ret->tag = LVAL_MGR;
    ret->num = INIT_SIZE;
    ret->next = NULL;
    ++ lval_managers;
  }

  assert( ret->tag == LVAL_MGR );
  if( ret->num > 1 ){
    lval* new_head = &ret[1];
    new_head->tag = LVAL_MGR;
    new_head->num = ret->num - 1;
    lval_push( &lval_manager[i]->reclaim, new_head );
  }

  ret->num = 0;
  ret->str = NULL;
  ret->asoc = NULL;
  ret->next = NULL;
  ret->fun = NULL;
  ret->tag = LVAL_NIL;
  return ret;
}

lval* stralloc( size_t bytes ){

  lval* ret = lalloc();
  unsigned int i;
  struct strmgr* curr = NULL;

  for( i = 0; i < strbuffs && ret->str == NULL; ++i ){

    curr = strbuff[i];
    if( ( ( INIT_CHAR - 1 ) - curr->next ) > bytes ){
      ret->str = &curr->buff[curr->next];
    }
  }

  if( ret->str == NULL ){

    strbuff = realloc( strbuff, sizeof(struct strmgr)*( strbuffs + 1 ) );
    curr = strbuff[i];
    curr->free = 0;
    curr->next = 0;
    curr->buff = malloc( INIT_CHAR * sizeof( char ) );
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

lval* lval_num( long x ){

  lval* ret = lalloc();
  ret->tag = LVAL_NUM;
  ret->num = x;
  return ret;
}

lval* lval_err( char* msg ){

  int len = strlen( msg ) + 1;
  lval* ret = stralloc(len);
  ret->tag = LVAL_ERR;
  strncpy( ret->str, msg, len );
  return ret;
}

lval* lval_sym( char* contents ){

  int len = strlen( contents ) + 1;
  lval* ret = stralloc( len );
  ret->tag = LVAL_SYM;
  strncpy( ret->str, contents, len );
  ret->next = NULL;
  return ret;
}

lval* lval_sxpr( ){

  lval* ret = lalloc();
  ret->tag = LVAL_SXPR;
  ret->num = 0;
  ret->next = NULL;
  ret->asoc = NULL;
  return ret;
}
lval* lval_qxpr( ){

  lval* ret = lalloc();
  ret->tag = LVAL_QXPR;
  ret->num = 0;
  ret->next = NULL;
  ret->asoc = NULL;
  return ret;
}

void ldrop( lval* ptr ){

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
    if( (lval*)( lval_manager[i] + 1 ) < ptr &&
        (lval*)( lval_manager[i] + 1 ) + INIT_SIZE > ptr ){
      lval_push( &lval_manager[i]->reclaim, ptr );
      return;
    }
  }

  assert( false /*Where did you get that pointer? Not from here*/);
}

void add_builtin( lenv* env, const char* sym, lbuiltin* function ){

  lval* symbol = search_env( env, sym );
  if( symbol->tag != LVAL_ERR ){
    if( symbol->tag == LVAL_FN ){

      ldrop( symbol->asoc );
      symbol->asoc = NULL;
    }

  }else{

    size_t len = strlen( sym ) + 1;
    ldrop( symbol );
    symbol = stralloc( len );
    strncpy( symbol->str, sym, len );
  }

  symbol->tag = LVAL_BUILTIN;
  symbol->fun = function;

  lval_push( &env->data, symbol );
}

lval* search_env( lenv* env, const char* sym ){

  lval* curr = env->data;

  while( curr != NULL ){

    if( strcmp( curr->str, sym ) ){
      return curr;
    }
    curr = curr->next;
  }
  char err[256];
  sprintf( err, "Symbol \"%s\" not bound in this environment", sym );
  return lval_err( err );
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
