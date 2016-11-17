
#ifndef _LVAL_H
#define _LVAL_H

#include <stdbool.h>
#include <lval_type.h>

bool lval_type_eq( lval*, lval* );
bool lval_eq( lval*, lval* );
bool lval_expr_eq( lval*, lval* );

lval *lval_num( long );
lval *lval_err( char* );
lval *lval_sym( char* );
lval *lval_sxpr();
lval *lval_qxpr();

void _lval_add( lval**, lval* );

lval *lval_cp( lval* );
lval *lval_cp_expr( lval* );

#endif//_LVAL_H