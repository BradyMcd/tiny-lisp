
#ifndef _MEMORY_H
#define _MEMORY_H

#define INIT_SIZE 4096

void* alloc( size_t bytes );
void drop( void* loc );
void flush_memory();

#endif//_MEMORY_H
