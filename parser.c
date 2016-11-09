
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

#include <mpc/mpc.h>

#include <parser.h>
#include <memory.h>

static mpc_parser_t* Num;
static mpc_parser_t* Sym;
static mpc_parser_t* Expr;
static mpc_parser_t* Sxpr;
static mpc_parser_t* Qxpr;
static mpc_parser_t* Lisp;

/*
 * Read step
 */
lval* _lval_read_num( mpc_ast_t* node ){

  errno = 0;
  long num = strtol( node->contents, NULL, 0 );
  if( errno == 0 ){
    return lval_num( num );
  }else if( errno == ERANGE ){
    char* err = "Number out of range";
    return lval_err( err );
  }else if( errno == EINVAL ){
    char* err = "Invalid digits found";
    return lval_err( err );
  }

  /*UNREACHABLE*/
  char* err = "Unknown error, invalid code path reached"
    " while converting number";
  return lval_err( err );
}

void _lval_add( lval** node, lval* new ){

  if( *node == NULL ){

    *node = new;
    return;
  }

  _lval_add( &((*node)->next), new );
}

lval* _read_ast( mpc_ast_t* node ){

  if( strstr( node->tag, "num" ) ){ return _lval_read_num( node ); }
  if( strstr( node->tag, "sym" ) ){ return lval_sym( node->contents ); }

  lval* ret = NULL;
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

      if( strcmp( node->children[i]->tag, "regex" ) == 0 )
        continue;
      else
        _lval_add( &ret->asoc, _read_ast( node->children[i] ) );
    }
    }
  }
  return ret;
}

/*
 *Eval step
 */

void lval_print( lval* node );
void _lval_recurse_print( lval* node ){

  if( node != NULL ){
    printf( " " );
    lval_print( node );
    _lval_recurse_print( node->next );
  }
}

void lval_print( lval* node ){

  switch( node->tag ){
  case LVAL_SXPR:
    printf( "(" );
    _lval_recurse_print( node->asoc );
    printf( " )" );
    break;
  case LVAL_QXPR:
    printf( "{" );
    _lval_recurse_print( node->asoc );
    printf( " }" );
    break;
  case LVAL_NUM:
    printf( "%li", node->num );
    break;
  case LVAL_SYM:
    printf( "%s", node->str );
    break;
  case LVAL_ERR:
    printf( "Error: %s", node->str );
    break;
  default:
    printf( "Somehow, something went wrong. Tag: %i", node->tag );
    /*THESE DON'T EXIST JUST YET
  case LVAL_BUILTIN:
    printf( "<Builtin Function>" );
    break;
  case LVAL_FN:
    printf( "<Function>" );
    break;
  case LVAL_VAR:
    printf( "%s = %li", node->str, node->num );
    break;
    */
  }
}

lval* read_string( const char* source, const char* string ){

  mpc_result_t result;
  lval* retval;

  if( mpc_parse( source, string, Lisp, &result ) ){

    retval = _read_ast( result.output );
    mpc_ast_delete( result.output );
  }else{
    mpc_err_print( result.error );
    mpc_err_delete( result.error );
    char* err = "Parser reported an error";
    retval = lval_err( err );
  }
  return retval;
}

void lval_drop( lval* ptr ){

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
