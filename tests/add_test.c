
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

#include <stdio.h>
#include <malloc.h>

#include <lval.h>
#include <parser.h>


lval *add( lenv *env, lval *value ){

  if( lval_type_of( value ) != LVAL_NUM ){
    return lval_err( "Found an invalid value for this implementation of +" );
  }

  lval *rv;
  if( lval_next_of( value ) != NULL ){
    lval *next = add( env, lval_take_next( value ) );

    if( lval_is_type( next, LVAL_ERR ) ){
      lval_drop( value );
      return next;
    }

    rv = lval_num( lval_num_of( next ) + lval_num_of( value ) );
    lval_drop( next );
  }else{
    rv = lval_cp( value );
  }
  lval_drop( value );
  return rv;
}

int main( ){

  parser_init();

  lenv *env = init_env();
  lval *result;

  result = add_builtin( env, "+", add );

  result = read_string( "add_test", "+ 2 3" );
  lval_fprint( stdout, NULL, "\n", result );
  result = eval_expr( env, result );
  lval_fprint( stdout, NULL, "\n", result );
  lval_drop( result );

  result = read_string( "add_test", "+ 1 1 1 1" );
  lval_fprint( stdout, NULL, "\n", result );
  result = eval_expr( env, result );
  lval_fprint( stdout, NULL, "\n", result );
  lval_drop( result );

  result = read_string( "add_test", "(+ 2 3 (+ 3 3))" );
  lval_fprint( stdout, NULL, "\n", result );
  result = eval_expr( env, result );
  lval_fprint( stdout, NULL, "\n", result );
  lval_drop( result );

  result = read_string( "add_test", "(+ 1 NaN)"  );
  lval_fprint( stdout, "\n", "\n", result );
  result = eval_expr( env, result );
  lval_fprint( stdout, NULL, "\n", result );
  lval_drop( result );

  free( env );
  parser_mem_cleanup();

  return 0;
}
