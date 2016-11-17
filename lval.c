
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

#include <memory.h>
#include <lval.h>

bool lval_type_eq( lval *a, lval *b ){

  if( a == NULL ){
    if( b == NULL ){
      return true;
    }else{
      return false;
    }
  }
  if( a->tag == b->tag ){
    return true;
  }
  return false;
}

bool lval_expr_eq( lval *a, lval *b );
bool lval_eq( lval *a, lval *b ){

  if( lval_type_eq( a, b ) ){
    if( a == NULL ){ return true; }
    switch( a->tag ){
    case LVAL_SYM:
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

  bool rv = true;
  lval *al = a;
  lval *be = b;

  while( rv && al != NULL ){

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

lval *lval_sym( char *contents ){

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

lval *search_env( lenv*, const char* );
void add_builtin( lenv *env, const char *sym, lbuiltin *function ){

  lval *symbol = search_env( env, sym );
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

lval *search_env( lenv *env, const char *sym ){

  lval *curr = env->data;

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
