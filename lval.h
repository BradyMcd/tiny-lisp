
#ifndef _LVAL_H
#define _LVAL_H

#include <stdbool.h>
#include <lval_type.h>

enum TAG lval_type_of( lval* );
bool lval_is_type( lval*, enum TAG );

bool lval_type_eq( lval*, lval* );
bool lval_eq( lval*, lval* );
bool lval_expr_eq( lval*, lval* );

lval *lval_num( long );
lval *lval_err( char* );
lval *lval_sym( char* );
lval *lval_sxpr();
lval *lval_qxpr();

long lval_num_of( lval* );
char *lval_sym_of( lval* );
lval *lval_asoc_of( lval* );
lval *lval_next_of( lval* );

lbuiltin lval_call( lval* );

lval *lval_pop( lval** );
void lval_push( lval**, lval* );
void lval_expr_add( lval*, lval* );

lval *lval_cp( lval* );
lval *lval_cp_expr( lval* );

lval *add_builtin( lenv*, const char*, lbuiltin );
lval *search_env( lenv*, const char* );

#endif//_LVAL_H
