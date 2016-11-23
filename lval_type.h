
#ifndef _LVAL_TYPE_H
#define _LVAL_TYPE_H

typedef struct lval lval;
typedef struct lenv{
  lval* data;
}lenv;

typedef lval*(*lbuiltin)(lenv*, lval*);

typedef struct lval{

  int tag;

  long num;
  char* str;

  struct lval* asoc;
  lbuiltin fun;

  struct lval* next;
}lval;

#endif//_LVAL_TYPE_H
