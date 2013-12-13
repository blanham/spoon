#ifndef _KERNEL_PHYSMEM_H
#define _KERNEL_PHYSMEM_H

#include <inttypes.h>
#include <multiboot.h>

void init_memory( multiboot_info_t *mboot );

void* memory_alloc( int pages );
void memory_free( int pages, void *position );

void show_memory_map();

void internal_reserve( uint32_t start, uint32_t end, int used );

#endif
