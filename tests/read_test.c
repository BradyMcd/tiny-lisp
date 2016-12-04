
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
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <parser.h>

int test_string( char *str, bool expect ){

  char res[512];

  printf("Reading: %s\n", str);

  lval *_rv = read_string( "read_test", str );

  assert( lval_sprint( res, 512, NULL, NULL, _rv ) == 0 );
  printf( "%s\n", res );
  if( lval_is_type( _rv, LVAL_ERR ) ){
    if( expect == false ){
      printf( "Expected parsing failure: Passed\n" );
    }else{
      printf( "Expected parsing failure: Failed\n" );
      lval_drop( _rv );
      return 1;
    }
  }else{

    if( strcmp( str, res ) == 0 && expect ){
      printf( "Expected parsing success: Passed\n" );
    }else if( expect ){
      printf( "Expected parsing success: Failed\n" );
    }else{
      printf( "Expected to be run in a sane environment ... \n" );
    }
  }

  lval_drop( _rv );
  return 0;
}

int main( ){

  int ret = 0;
  const int tests = 5;
  parser_init();
  ret += test_string( "( + 2 3 )", true );
  ret += test_string( "{ + 2 3 }", true );

  ret += test_string( "{ a abc ss ", false );
  ret += test_string( "ab 2 2 3 { some a b c }", true );
  ret += test_string( "- 1 2 )", false );

  parser_mem_cleanup();


  printf( "\nTesting reports %i passed tests out of %i\n", tests - ret, tests );
  return ret;
}
