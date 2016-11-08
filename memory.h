
#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdlib.h>

#define INIT_SIZE 4096
#define INIT_CHAR 8192

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

typedef struct lval lval;
typedef struct lenv{
  lval* data;
}lenv;

typedef lval*(*lbuiltin)(lenv*, lval*);

typedef struct lval{

  enum TAG tag;

  long num;
  char* str;

  struct lval* asoc;
  lbuiltin* fun;

  struct lval* next;
}lval;

lval* lval_pop( lval** );
void lval_push( lval**, lval* );

lval* lval_num( long );
lval* lval_err( char* );
lval* lval_sym( char* );
lval* lval_sxpr();
lval* lval_qxpr();

void ldrop( lval* ptr );

/*Add environment guts here*/
void add_builtin( lenv*, const char*, lbuiltin* );
lval* search_env( lenv*, const char* );

void lval_cleanup();

#endif//_MEMORY_H
