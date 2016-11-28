
#ifndef _LVAL_H
#define _LVAL_H

#include <stdbool.h>

typedef struct lval lval;
typedef struct lenv lenv;

enum TAG{
  LVAL_NIL,
  LVAL_MGR,
  LVAL_ERR,
  LVAL_NUM,
  LVAL_SYM,
  LVAL_VAR,
  LVAL_FN,
  LVAL_BUILTIN,
  LVAL_SXPR,
  LVAL_QXPR,
};
typedef lval*(*lbuiltin)(lenv*, lval*);

enum TAG lval_type_of( lval* );
bool lval_is_type( lval*, enum TAG );

bool lval_type_eq( lval*, lval* );
bool lval_eq( lval*, lval* );
bool lval_expr_eq( lval*, lval* );

lval *lval_num( long );
lval *lval_err( char* );
lval *lval_sym( const char* );
lval *lval_sxpr();
lval *lval_qxpr();
lval *lval_nil();

long lval_num_of( lval* );
char *lval_sym_of( lval* );
lval *lval_asoc_of( lval* );
lval *lval_next_of( lval* );

lval *lval_take_asoc( lval* );
lval *lval_take_next( lval* );

lbuiltin lval_call( lval* );

lval *lval_pop( lval** );
void lval_push( lval**, lval* );
void lval_expr_add( lval*, lval* );
void lval_cat( lval*, lval* );

lval *lval_cp( lval* );
lval *lval_cp_expr( lval* );

#endif//_LVAL_H
