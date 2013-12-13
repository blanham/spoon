#ifndef _KERNEL_UALLOC_H
#define _KERNEL_UALLOC_H


#define UALLOC_NORELEASE		1

struct allocated_memory
{
	unsigned int pages;
	unsigned int flags;

	unsigned int phys;
	unsigned int virt;

	int shared;
	
	struct process *parent;
	struct allocated_memory* next;
	struct allocated_memory* prev;
} __attribute__ ((packed));




unsigned int user_alloc( struct process *proc, unsigned int pages );
unsigned int user_free( struct process *proc, uint32_t address );
int          user_size( struct process *proc, uint32_t pointer );
unsigned int user_location( struct process *proc, uint32_t pointer );

unsigned int user_map( struct process *proc, unsigned int location, 
							unsigned int pages );


int user_ensure( struct process *proc, uint32_t address, unsigned int pages );

#endif

