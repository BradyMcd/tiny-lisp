
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

#include <parser.h>

int test_string( char* str, bool expect ){
  printf("Reading: %s\n", str);
  lval* _rv = read_string( "Internal", str );
  lval_print( _rv );
  printf("\nExpected %s.\n", expect?"matching output":"an error");

  lval_drop( _rv );
  return 0;
}

int main( ){

  int ret = 0;
  parser_init();
  test_string( "( + 2 3 )", true );
  test_string( "{ + 2 3 }", true );

  test_string( "{ a abc ss ", false );
  test_string( "ab 2 2 3 { some a b c }", false );
  test_string( "- 1 2 )", false );

  parser_mem_cleanup();

  return ret;
}
