
#ifndef _PARSER_H
#define _PARSER_H

#include <stddef.h>

#include <lval.h>

int lval_sprint( char*, size_t, char*, char*, lval* );
void lval_fprint( FILE*, char*, char*, lval* );

lval* read_string( const char*, const char* );

void lval_drop( lval* );

lval *eval_expr( lenv*, lval* );

void parser_init();

void parser_cleanup();
void parser_mem_cleanup();

#endif//_PARSER_H
