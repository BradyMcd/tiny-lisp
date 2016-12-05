
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
#include <stdio.h>

#include <lval_type.h>

#include <memory.h>
#include <lval.h>

#define RECYCLE_TESTS 10

enum MEMORY_ERRORS{
  ERR_BOUNDS = 0x1,
  ERR_NEWCHUNK = 0x2,
  ERR_RECLAIM = 0x4,
  ERR_RECLAIM_CHUNK = 0x8,
};

enum MEMORY_ERRORS fill_chunk(){

  int i;
  enum MEMORY_ERRORS rv = 0;
  lval *buffer[INIT_SIZE + 1];

  printf( "Allocating... " );

  buffer[0] = lalloc();

  for( i = 1; i < INIT_SIZE; ++i ){
    buffer[i] = lalloc();
    /*If this condition fails that means lalloc has returned a pointer
     *outside of the expected buffer of INIT_SIZE lvals
     */
    if( !( buffer[0] < buffer[i] &&
           buffer[i] < buffer[0] + INIT_SIZE*sizeof(lval) ) ){
      rv |= ERR_BOUNDS;
    }
  }
  buffer[INIT_SIZE] = lalloc();
  /*And if this condition fails that means lalloc hasn't made a new buffer
   *and my pointer math is off.
   */
  if( buffer[0] < buffer[INIT_SIZE] &&
      buffer[INIT_SIZE] < buffer[0] + INIT_SIZE*sizeof(lval) ){
    rv |= ERR_NEWCHUNK;
    printf( "Error allocating the first member of a new chunk\n"
            "Attempting to drop all values and exiting.\n");
  }

  //This condition fails if something else is on the reclaim list
  ldrop( buffer[0] );
  if( !( buffer[0]->next == NULL ) ){
    rv |= ERR_RECLAIM;
  }

  for( i = 1; i < INIT_SIZE; ++i ){
    ldrop( buffer[i] );
    if( !( buffer[i]->next != NULL ) ){
      rv |= ERR_RECLAIM;
    }
  }
  ldrop( buffer[INIT_SIZE] );

  return rv;
}

/*This currently tests the recycling list of the memory module
 *It is built specifically for how said list is structured, ideally I'll
 *be rebuilding this module so as to be more generic in future.
 */
int recycle_test(){

  int rv = 0;
  int i;
  lval *buffer[RECYCLE_TESTS];

  for( i = 0; i < RECYCLE_TESTS; ++i ){

    buffer[i] = lalloc();
  }

  for( i = 0; i < RECYCLE_TESTS; ++i ){

    ldrop( buffer[i] );
  }

  for( i = 9; i >= 0; --i ){
    /*at each step either the pointer yielded by lalloc will be equal to
     *the previously dropped pointer on the list or something strange is
     *happening
     */
    rv += buffer[i] != lalloc();
  }
  return rv;
}

int main(){

  printf( "Testing... \n" );

  int err;
  printf( "Chunking system: " );
  err = fill_chunk();
  printf( (err==0)?"No errors reported\n":"Error: %i\n", err );

  if( err & ERR_NEWCHUNK ){
    return 1;
  }

  printf( "Reseting memory...\n" );
  lval_cleanup();

  printf( "Recycling system: Reports %i unexpected effects of %i\n",
          recycle_test(), RECYCLE_TESTS );

  lval_cleanup();

  return 0;
}
