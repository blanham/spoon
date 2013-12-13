#ifndef _KERNEL_EXEC_H
#define _KERNEL_EXEC_H 		


#include <types.h>
#include <process.h>

void exec_map_in( vmmap *map, 
		 			 uintptr source, 
		 			 uintptr destination, 
		  			 uintptr size,
		  			 uint32 copy );
		  
int exec_memory_region( int pid,
	    				char * name, 
                   		uintptr start, 
	    				uintptr end, 
	    				char *parameters );

#endif
