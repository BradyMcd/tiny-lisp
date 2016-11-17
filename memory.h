
#ifndef _MEMORY_H
#define _MEMORY_H

#include <stddef.h>
#include <lval_type.h>

#define INIT_SIZE 4096
#define INIT_CHAR 8192

lval *lalloc();
lval *stralloc( size_t );

void ldrop( lval* ptr );

void lval_cleanup();

#endif//_MEMORY_H
