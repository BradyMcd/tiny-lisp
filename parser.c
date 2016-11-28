
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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <mpc/mpc.h>

#include <memory.h>
#include <lval.h>
#include <environment.h>
#include <parser.h>

static mpc_parser_t *Num;
static mpc_parser_t *Sym;
static mpc_parser_t *Expr;
static mpc_parser_t *Sxpr;
static mpc_parser_t *Qxpr;
static mpc_parser_t *Lisp;

/*
 * Read step
 */
lval *_lval_read_num( mpc_ast_t *node ){

  errno = 0;
  long num = strtol( node->contents, NULL, 0 );
  if( errno == 0 ){
    return lval_num( num );
  }else if( errno == ERANGE ){
    char *err = "Number out of range";
    return lval_err( err );
  }else if( errno == EINVAL ){
    char *err = "Invalid digits found";
    return lval_err( err );
  }

  /*UNREACHABLE*/
  char *err = "Unknown error, invalid code path reached"
    " while converting number";
  return lval_err( err );
}

lval *_read_ast( mpc_ast_t *node ){

  if( strstr( node->tag, "num" ) ){ return _lval_read_num( node ); }
  if( strstr( node->tag, "sym" ) ){ return lval_sym( node->contents ); }

  lval *ret = NULL;
  int i;
  if( strcmp( node->tag, ">" /*mpc marks root with ">"*/ ) == 0 ||
      strstr( node->tag, "sxpr" ) ){
    ret = lval_sxpr();
  }else if( strstr( node->tag, "qxpr" ) ){
    ret = lval_qxpr();
  }

  for( i=0; i<node->children_num; ++i ){

    switch( node->children[i]->contents[0] ){
    case '(' : { continue; }
    case ')' : { continue; }
    case '{' : { continue; }
    case '}' : { continue; }
    default : {

      if( strcmp( node->children[i]->tag, "regex" ) == 0 ){
        continue;
      }else{
        lval_expr_add( ret, _read_ast( node->children[i] ) );
      }
    }
    }
  }
  return ret;
}

/*
 *Eval step
 */

lval *eval_expr( lenv *env, lval *n ){
  if( n == NULL ){ return NULL; }

  lval *node = n;
  lval *work;
  lval *tail;
  lval *next = eval_expr( env, lval_take_next( node ) );

  if( next != NULL && lval_is_type( next, LVAL_ERR ) ){
    ldrop( node );
    return next;
  }

  switch( lval_type_of( node ) ){
  case LVAL_SXPR:

    work = eval_expr( env, lval_take_asoc( node ) );
    ldrop( node ); node = work;
    if( lval_is_type( node, LVAL_SYM ) ){

      work = search_env( env, lval_sym_of( node ) );
      switch( lval_type_of( work ) ){
      case LVAL_FN:

        break;
      case LVAL_BUILTIN:

        tail = eval_expr( env, lval_take_next( node ) );
        ldrop( node );
        node = lval_call( work )( env, tail );
        if( next!= NULL ){ lval_push( &next, node ); }
        //tail should probably have resolved to NULL here
        return node == NULL?lval_nil():node;
        break;
      case LVAL_VAR:

        node = lval_cp_expr( lval_asoc_of( work ) );
        if( next!= NULL ){ lval_push( &next, node ); }
        return node;
        break;
      case LVAL_ERR:

        if( next != NULL ){ ldrop( next ); }
        ldrop( node );
        return work;
        break;
      default:
        //This should be unreachable. Perhaps I should have different tag types for environment lvals, memory lvals and lvals the language can actually see.
        break;
      }
    }else{

      return node;
    }
    break;
  case LVAL_QXPR:
  case LVAL_SYM:
  case LVAL_NUM:
    if( next!= NULL ){ lval_push( &next, node ); }
    return node;
    break;
  default:
    return lval_err( "I can't imagine how you might get here. Until I get here of course and step through in gdb" );
  }
  return lval_err( "Again, I imagine this is unreachable, but gdb is quite the imagination enhancer" );
}

/*
 *Printing Lisp values
 */
#define WRITE_BUFFER( LOC, LEFT, SRC )   \
  { int _strlen = strlen( SRC );        \
    if( LEFT < _strlen ){               \
      strncpy( LOC, SRC, _strlen );     \
      LOC += _strlen;                   \
      LEFT -= _strlen;                  \
    }else{ return 1; }}

int _lval_sprint( char *dest, size_t n, char *od, char *cd, lval *node ){

  char buff[128];
  char *loc = dest;
  int left = n;

  if( od != NULL ){
    WRITE_BUFFER( loc, left, od )
  }

  switch( lval_type_of( node ) ){
  case LVAL_SXPR:
    _lval_sprint( loc, left, "( ", " )", lval_asoc_of( node ) );
    break;
  case LVAL_QXPR:
    _lval_sprint( loc, left, "{ ", "}", lval_asoc_of( node ) );
    break;
  case LVAL_NUM:
    sprintf( buff, "%li ", lval_num_of( node ) );
    WRITE_BUFFER( loc, left, buff )
    break;
  case LVAL_SYM:
    WRITE_BUFFER( loc, left, lval_sym_of( node ) )
    WRITE_BUFFER( loc, left, " " )
    break;
  case LVAL_ERR:
    /*This is technically a much different code path,
     *I'll probably split these print functions in 3 when environment works*/
    WRITE_BUFFER( loc, left, lval_sym_of( node ) )
    break;
  default:
    fprintf( stderr, "Somehow, something went wrong. Tag: %i",
             lval_type_of( node) );
    return 1;
    break;
  }

  if( cd != NULL ){
    WRITE_BUFFER( loc, left, cd )
  }

  if( lval_next_of( node ) != NULL )
    return lval_sprint( loc, left, NULL, NULL, lval_next_of( node ) );

  return 0;
}
/*NOTE: If this function returns 1 it has possibly left the dest string in an
  invalid state*/
int lval_sprint( char *dest, size_t n, char *od, char *cd, lval *node){
  memset( dest, 0, n );
  return _lval_sprint( dest, n, od, cd, node );
}

void _lval_fprint( FILE* stream, char *od, char *cd, lval *node ){

  if( od != NULL ){
    fputs( od, stream );
  }

  switch( lval_type_of( node ) ){
  case LVAL_SXPR:
    _lval_fprint( stream, "( ", ")", lval_asoc_of( node ) );
    break;
  case LVAL_QXPR:
    _lval_fprint( stream, "{ ", "}", lval_asoc_of( node ) );
    break;
  case LVAL_NUM:
    fprintf( stream, "%li ", lval_num_of( node ) );
    break;
  case LVAL_SYM:
    fprintf( stream, "%s ", lval_sym_of( node ) );
    break;
  case LVAL_NIL:
    fprintf( stream, "nil " );
    break;
  case LVAL_ERR:
    /*Again, in states where an error is present things should be very
      different. This wants a rewrite after environment and error handling*/
    fprintf( stream, "Error: %s", lval_sym_of( node ) );
    break;
  default:
    fprintf( stderr, "Somehow, something went wrong. Tag: %i",
             lval_type_of( node ) );
  }

  if( lval_next_of( node ) != NULL )
    _lval_fprint( stream, NULL, NULL, lval_next_of( node ) );

  if( cd != NULL )
    fputs( cd, stream );
}
void lval_fprint( FILE* stream, char *od, char *cd, lval *node ){
  if( lval_type_of( node ) == LVAL_SXPR ){
    _lval_fprint( stream, od, cd, lval_asoc_of( node ) );
  }else{
    //This is almost definitely the error path
    _lval_fprint( stream, od, cd, node );
  }
}

lval *read_string( const char *source, const char *string ){

  mpc_result_t result;
  lval *retval;

  if( mpc_parse( source, string, Lisp, &result ) ){

    retval = _read_ast( result.output );
    mpc_ast_delete( result.output );
  }else{
    mpc_err_print( result.error );
    mpc_err_delete( result.error );
    char *err = "Parser reported an error";
    retval = lval_err( err );
  }
  return retval;
}

void lval_drop( lval *ptr ){

  ldrop( ptr );
}

void parser_init(){

  Num = mpc_new( "num" );
  Sym = mpc_new( "sym" );
  Expr = mpc_new( "expr" );
  Qxpr = mpc_new( "qxpr" );
  Sxpr = mpc_new( "sxpr" );
  Lisp = mpc_new( "lisp" );

  mpca_lang( MPCA_LANG_DEFAULT,
             " \
              num : /-?[0-9]+/ ; \
              sym : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; \
              expr: <num> | <sym> | <sxpr> | <qxpr> ; \
              qxpr: '{' <expr>* '}' ; \
              sxpr: '(' <expr>* ')' ; \
              lisp: /^/ <expr>* /$/ ; \
             ",
             Num, Sym, Expr, Qxpr, Sxpr, Lisp );

}

void parser_cleanup(){

  mpc_cleanup( 6, Num, Sym, Expr, Sxpr, Qxpr, Lisp );
}

void parser_mem_cleanup(){

  parser_cleanup();
  lval_cleanup();
}
