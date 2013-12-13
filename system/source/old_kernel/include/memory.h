#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <types.h>



void init_memory( int totalPages );
void show_memory_map();

void* memory_alloc( int pages );
void* memory_multiple_alloc( int pages );
void  memory_free( int pages, void *position );

unsigned int free_memory();
unsigned int total_memory();


void memory_internalSet( int block, int used );

#endif

