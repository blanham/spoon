#ifndef _KERNEL_UALLOC_H
#define _KERNEL_UALLOC_H


#define UALLOC_NORELEASE		1

struct allocated_memory
{
	unsigned int pages;
	unsigned int flags;

	uintptr phys;
	uintptr virt;

	int shared;
	
	struct process *parent;
	struct allocated_memory* next;
	struct allocated_memory* prev;
} __attribute__ ((packed));




uintptr user_alloc( struct process *proc, uint32 pages );
uintptr user_free( struct process *proc, uint32 address );
int32   user_size( struct process *proc, uint32 pointer );
uintptr user_location( struct process *proc, uint32 pointer );

uintptr user_map( struct process *proc, uintptr location, uint32 pages );


#endif

