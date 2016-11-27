
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

#define BI_BB_START( C_NAME, TEMP_INIT )                  \
  lval *C_NAME( lenv *env, lval *value ){                 \
  lval *rv = NULL; lval *next;                            \
  lval *temp = TEMP_INIT;                                 \
  lval *curr = value;                                     \
  while( curr != NULL && !lval_is_type( rv, LVAL_ERR ) ){ \
  switch( lval_type_of( curr ) ){

#define BI_BB_BODY( TYPE, BODY )                \
  case TYPE:                                    \
  BODY                                          \
  break;

#define BI_BB_END( C_NAME )                                             \
  case LVAL_ERR:                                                        \
  rv = value;                                                           \
  if( lval_next_of( value )!=NULL ){ lval_drop( lval_next_of( value ) );} \
  break;                                                                \
  default:                                                              \
  rv = lval_err( "Unimplemented value passed to function" ); \
  } next = lval_take_next( curr ); lval_drop( curr ); curr = next;      \
  } return rv; }

BI_BB_START( ladd, lval_num( 0 ) )
BI_BB_BODY( LVAL_NUM, {
    rv = lval_num( lval_num_of( temp ) + lval_num_of( curr ) );
    lval_drop( temp ); temp = rv;
  } )
BI_BB_END( ladd )

void eval_test( lenv* env, const char *string ){

  lval *result = read_string( "eval_test", string );
  lval_fprint( stdout, NULL, "\n", result );
  result = eval_expr( env, result );
  lval_fprint( stdout, NULL, "\n", result );
  lval_drop( result );
}

int main( ){

  parser_init();

  lenv *env = init_env();

  add_builtin( env, "+", ladd );

  eval_test( env, "(+ 3 4)" );
  eval_test( env, "(+ 1 1 1 1 1)" );
  eval_test( env, "{+ 1 2(+ 2 3)}" );
  eval_test( env, "(+ (+ 3 4) (+ 6 7) 2)" );
  eval_test( env, "(+ 1)" );
  eval_test( env, "(+)" );
  eval_test( env, "(+ 2 NaN)" );

  free( env );
  parser_mem_cleanup();

  return 0;
}
