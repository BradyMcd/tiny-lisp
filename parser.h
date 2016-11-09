
#ifndef _PARSER_H
#define _PARSER_H

typedef struct lval lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);

void lval_print( lval* );

lval* read_string( const char*, const char* );

void lval_drop( lval* );

void parser_init();

void parser_cleanup();
void parser_mem_cleanup();


#endif//_PARSER_H
