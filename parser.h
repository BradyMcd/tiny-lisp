
#ifndef _PARSER_H
#define _PARSER_H

#include <stddef.h>

typedef struct lval lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);

int lval_sprint( char*, size_t, char*, char*, lval* );
void lval_fprint( FILE*, char*, char*, lval* );

lval* read_string( const char*, const char* );

void lval_drop( lval* );

void parser_init();

void parser_cleanup();
void parser_mem_cleanup();


#endif//_PARSER_H