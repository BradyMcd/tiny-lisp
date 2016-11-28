
#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

typedef struct lval lval;
typedef struct lenv lenv;

#define BI_BB_START( C_NAME, TEMP_INIT )                  \
  lval *C_NAME( lenv *env, lval *value ){                 \
  lval *rv = NULL; lval *next;                            \
  lval *temp = TEMP_INIT;                                 \
  lval *curr = value;                                     \
  while( curr != NULL && !lval_is_type( rv, LVAL_ERR ) ){ \
  switch( lval_type_of( curr ) ){

#define BI_BB_BODY( TYPE, BODY )                \
  case TYPE:                                    \
  BODY                                          \
  break;

#define BI_BB_END( C_NAME )                                             \
  case LVAL_ERR:                                                        \
  rv = value;                                                           \
  if( lval_next_of( value )!=NULL ){                                    \
    lval_drop( lval_next_of( value ) );                                 \
  }                                                                     \
  break;                                                                \
  default:                                                              \
  rv = lval_err( "Unimplemented value passed to function" );            \
  } next = lval_take_next( curr ); lval_drop( curr ); curr = next;      \
  } return rv; }

lval *add_builtin( lenv*, const char*, lbuiltin );
lval *search_env( lenv*, const char* );

lenv *new_env( lenv* );
lenv *delete_env( lenv* );

#endif//_ENVIRONMENT_H
