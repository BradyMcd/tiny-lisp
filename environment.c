
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

#include <malloc.h>
#include <string.h>

#include <memory.h>
#include <lval_type.h>
#include <lval.h>

lval *search_env( lenv*, const char* );
lval *add_builtin( lenv *env, const char *sym, lbuiltin function ){

  lval *symbol = search_env( env, sym );
  lval *rv;
  if( symbol->tag != LVAL_ERR ){
    if( symbol->tag == LVAL_FN ){

      ldrop( symbol->asoc );
      symbol->asoc = NULL;
    }
    rv = lval_err( "Warning: Symbol previously bound");
  }else{
    symbol = lval_sym( sym );
  }

  symbol->tag = LVAL_BUILTIN;
  symbol->fun = function;

  lval_push( &env->data, symbol );

  return symbol;
}

lval *search_env( lenv *e, const char *sym ){

  lenv *env = e;
  lval *curr;

  while( env != NULL ){
    curr = env->data;
    while( curr != NULL ){

      if( strcmp( curr->str, sym ) == 0 ){
        return curr;
      }
      curr = curr->next;
    }
    env = env->parent;
  }
  char err[256];
  sprintf( err, "Symbol \"%s\" not bound in this environment", sym );
  return lval_err( err );
}

lenv *new_env( lenv *ptr ){

  lenv *new = malloc( sizeof( lenv ) );
  new->parent = ptr;
  new->data = NULL;
  return new;
}

lenv *delete_env( lenv *ptr ){
  lenv *rv = ptr->parent;
  ldrop( ptr->data );
  free( ptr );
  return rv;
}
