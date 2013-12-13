#include <types.h>
#include <strings.h>
#include <process.h>
#include <paging.h>


#define CON_SCREEN		0xB8000

int arch_save_console( void *data, int pages )
{
    memcpy( data , (char*)CON_SCREEN, (160*25) );
	return 0;
}


int arch_restore_console( void *data, int pages  )
{
 	memcpy( (char*)(CON_SCREEN), data , (160*25) );
	return 0;
}

																   
int arch_unmap_console( struct process *proc, void *data, int pages )
{
	uint32 location = (uint32)(proc->console_location);
	return page_direct_map( proc->map, (uint32)data, location, 1, USER_PAGE | READ_WRITE );
}

int arch_map_console( struct process *proc, void *data, int pages )
{
	uint32 location = (uint32)(proc->console_location);
	return page_direct_map( proc->map, CON_SCREEN, location, 1, USER_PAGE | READ_WRITE );
}

		 

	
