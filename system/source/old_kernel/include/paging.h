#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#include <types.h>


#define		READ_ONLY		1
#define		READ_WRITE		2
#define		USER_PAGE		4
#define		SYSTEM_PAGE		8
#define		SHARED			16



vmmap* new_vmmap();
int    delete_vmmap( vmmap* map );


int   set_vmmap( vmmap*  map );
int   get_vmmap( vmmap* map );


uintptr page_find_free( vmmap* map, 
					  uintptr start,
					  uintptr finish,
					  uintptr pages );

uintptr     page_ensure( vmmap* map,
			 	     	 uintptr start,
				     	 uintptr pages,
			   			 unsigned int type	);



/* Provide virtual memory (allocating physical memory) to the
 * map, somewhere between start and end for pages.
 */

uintptr     page_map_in( vmmap* map,
			 	     uintptr start,
			 	     uintptr end,
				     uintptr pages,
				     uintptr type );

/* Release the memory at the virtual address "start" for "pages"
 * amount of pages using the memory map.  This frees the physical
 * memory as well.
 */

int			page_map_out( vmmap* map, 
						  uintptr start, 
						  int pages );


/* Map in the physical memory into the map somewhere in the
 * region of start to end
 */

uintptr     page_provide( vmmap* map,
			 	     		uintptr physical,
			 	     		uintptr start,
			 	     		uintptr end,
				     		uintptr pages,
				     		uintptr type );

/* Map in the physical region of memory directly to start, for pages amount of pages */
int			page_direct_map( vmmap* map, uintptr physical, uintptr start, int pages, unsigned int type );


/* Release the pages at start address in the map but do not free the physical memory */

int			page_release( vmmap* map, uintptr start, int pages );




int page_copy_out( vmmap *map, uintptr dest, uintptr src, int bytes );
int page_copy_in(  vmmap* map, uintptr dest, uintptr src, int bytes );
 


#endif


