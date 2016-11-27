
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
#include <stdlib.h>

#include <lval_type.h>
#include <memory.h>
#include <lval.h>

enum TAG lval_type_of( lval* a ){
  if( a == NULL ){ return LVAL_NIL; }
  return a->tag;
}

bool lval_is_type( lval* a, enum TAG type ){
  if( lval_type_of( a ) == type ){ return true; }
  return false;
}

bool lval_type_eq( lval *a, lval *b ){
  assert( a != NULL );
  assert( b != NULL );

  return lval_is_type( a, b->tag );
}

bool lval_expr_eq( lval *a, lval *b );
bool lval_eq( lval *a, lval *b ){
  assert( a != NULL );
  assert( b != NULL );

  if( lval_type_eq( a, b ) ){
    switch( a->tag ){
    case LVAL_SYM:
    case LVAL_BUILTIN:
    case LVAL_FN:
      if( a->num == b->num ){

        return strncmp( a->str, b->str, a->num );
      }else{

        return false;
      }
      break;
    case LVAL_NUM:
      return a->num == b->num;
      break;
    case LVAL_ERR:
      return false;
      break;
    case LVAL_SXPR:
    case LVAL_QXPR:
      if( a->asoc == NULL || b->asoc == NULL ){ return false; }
      return lval_expr_eq( a->asoc, b->asoc );
      break;
    default:
      fprintf( stderr, "Are you lost? Comparing two lval of type tag: %d",
               a->tag );
      return false;
    }
  }

  return false;
}

bool lval_expr_eq( lval *a, lval *b ){
  assert( a != NULL );
  assert( b != NULL );

  bool rv = true;
  lval *al = a;
  lval *be = b;

  while( rv && al != NULL && be != NULL ){

    rv = lval_eq( al, be );
    al = al->next;
    be = be->next;
  }
  return rv;
}

lval *lval_num( long x ){

  lval *ret = lalloc();
  ret->tag = LVAL_NUM;
  ret->num = x;
  return ret;
}

lval *lval_err( char *msg ){

  int len = strlen( msg ) + 1;
  lval *ret = stralloc(len);
  ret->tag = LVAL_ERR;
  strncpy( ret->str, msg, len );
  return ret;
}

lval *lval_sym( const char *contents ){

  int len = strlen( contents ) + 1;
  lval *ret = stralloc( len );
  ret->tag = LVAL_SYM;
  strncpy( ret->str, contents, len );
  return ret;
}

lval *lval_sxpr( ){

  lval *ret = lalloc();
  ret->tag = LVAL_SXPR;
  return ret;
}
lval *lval_qxpr( ){

  lval *ret = lalloc();
  ret->tag = LVAL_QXPR;
  return ret;
}

lval *lval_nil(){

  lval *ret = lalloc();
  ret->tag = LVAL_NIL;
  return ret;
}

long lval_num_of( lval *node ){

  switch( node->tag ){
  case LVAL_NUM:
  case LVAL_SYM:
  case LVAL_ERR:
    return node->num;
  default:
    fprintf( stderr, "Trying to reach a number which does not exist" );
  }
  return 0;
}

char *lval_sym_of( lval *node ){

  switch( node->tag ){
  case LVAL_SYM:
  case LVAL_ERR:
  case LVAL_FN:
  case LVAL_BUILTIN:
    return node->str;
  default:
    fprintf( stderr, "Trying to reach a symbol which does not exist" );
  }
  return NULL;
}

lval *lval_asoc_of( lval *node ){

  switch( node->tag ){
  case LVAL_SXPR:
  case LVAL_QXPR:
  case LVAL_FN:
  case LVAL_VAR:
    return node->asoc;
  default:
    fprintf( stderr, "Trying to reach an asoc slot which does not exist" );
  }
  return NULL;
}

lval *lval_next_of( lval *node ){

  return node->next;
}

lval *lval_take_asoc( lval *node ){
  lval *asoc = node->asoc;

  node->asoc = NULL;

  return asoc;
}

lval *lval_take_next( lval *node ){
  lval *next = node->next;

  node->next = NULL;

  return next;
}

lbuiltin lval_call( lval *fn ){

  if( lval_type_of( fn ) == LVAL_BUILTIN )
    return fn->fun;

  fprintf( stderr, "Attempted to call a non-funtion" );
  return NULL;
}

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

void _lval_add( lval **node, lval *new ){

  if( *node == NULL ){

    *node = new;
    return;
  }

  _lval_add( &((*node)->next), new );
}

void lval_expr_add( lval *node, lval *new ){

  _lval_add( &node->asoc, new );
}

void lval_cat( lval *node, lval *new ){

  _lval_add( &node->next, new );
}

lval *lval_cp_expr( lval *node );
lval *lval_cp( lval *node ){
  lval *new = NULL;
  if( node == NULL ){ return NULL; }
  switch( node->tag ){
  case LVAL_ERR:
    new = lval_err( node->str );
    break;
  case LVAL_SYM:
    new = lval_sym( node->str );
    break;
  case LVAL_NUM:
    new = lval_num( node->num );
    break;
  case LVAL_SXPR:
    new = lval_sxpr();
    _lval_add( &node, lval_cp_expr( node->asoc ) );
    break;
  case LVAL_QXPR:
    new = lval_qxpr();
    _lval_add( &new, lval_cp_expr( node->asoc ) );
    break;
  default:
    new = lval_err( "Copying that kind of value is not implemented" );
    break;
  }
  return new;
}

lval *lval_cp_expr( lval* node ){

  lval *curr = node;
  lval *new = lval_cp( curr );

  curr = curr->next;
  while( curr ){
    _lval_add( &new, lval_cp( curr ) );

    curr = curr->next;
  }

  return new;
}

lenv *init_env(  ){
  lenv *rv = malloc( sizeof( lenv ) );
  rv->data = NULL;
  return rv;
}

lval *search_env( lenv*, const char* );
lval *add_builtin( lenv *env, const char *sym, lbuiltin function ){

  lval *symbol = search_env( env, sym );
  lval *rv;
  if( symbol->tag != LVAL_ERR ){
    if( symbol->tag == LVAL_FN ){

      ldrop( symbol->asoc );
      symbol->asoc = NULL;
    }
    char *msg = "Warning: Symbol previously bound";
    rv = lval_err( msg );
  }else{
    symbol = lval_sym( sym );
  }

  symbol->tag = LVAL_BUILTIN;
  symbol->fun = function;

  lval_push( &env->data, symbol );

  return symbol;
}

lval *search_env( lenv *env, const char *sym ){

  lval *curr = env->data;

  while( curr != NULL ){

    if( strcmp( curr->str, sym ) == 0 ){
      return curr;
    }
    curr = curr->next;
  }
  char err[256];
  sprintf( err, "Symbol \"%s\" not bound in this environment", sym );
  return lval_err( err );
}
