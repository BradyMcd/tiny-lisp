
#ifndef _LVAL_TYPE_H
#define _LVAL_TYPE_H

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

#endif//_LVAL_TYPE_H
