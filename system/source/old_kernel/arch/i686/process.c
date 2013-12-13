#include <configure.h>
#include <macros.h>
#include <types.h>
#include <alloc.h>
#include <process.h>
#include <memory.h>

#include "process_data.h"

int arch_new_process( struct process *p )
{
	struct process_data *pd = malloc( sizeof( struct process_data ) );

	
		pd->bogomips = 0;

		
	p->data = pd;
	return 0;
};


int arch_delete_process( struct process *p )
{
	free( p->data );
	
	p->data = NULL;
	return 0;
}

