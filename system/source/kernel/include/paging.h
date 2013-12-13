#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#include <inttypes.h>

#define		READ_ONLY		1
#define		READ_WRITE		2
#define		USER_PAGE		4
#define		SYSTEM_PAGE		8
#define		SHARED			16




// ------------------- PAGE CONSTANTS

//               ----- DEFINES

#define PAGE_PRESENT			(1)
#define PAGE_WRITE_THROUGH		(1<<3)
#define PAGE_WRITE_BACK			0x0
#define PAGE_CACHE_ENABLE		0x0
#define PAGE_CACHE_DISABLE		(1<<4)
#define PAGE_READ_ONLY			0x0
#define PAGE_RDWR				(1<<1)
#define PAGE_USER				(1<<2)
#define PAGE_SUPERVISOR			0x0
#define PAGE_GLOBAL				(1<<8)
#define PAGE_DIRTY				0x40 // only for PTE
#define PAGE_ACCESSED			(1<<5)
#define PAGE_LOCAL				0x0
#define PAGE_4MB				(1<<7) // only for PDE's
#define PAGE_4KB				0x0  // only for PDE'S


void init_paging();

void load_paging( unsigned int cpu_id );


extern uint32_t page_directory[1024];

#define page_table_of(address)  (((address)>>22) & 0x3FF) 
#define page_entry_of(address)  (((address)>>12) & 0x3FF) 


int   		set_map( uint32_t*  map );
uint32_t*   get_map();


uint32_t* new_page_directory();
void 	  delete_page_directory( uint32_t* map );

// ---------------------------------------------------------------------

uint32_t page_find_free( uint32_t* map, 
					  uint32_t start,
					  uint32_t finish,
					  uint32_t pages );

int     page_ensure( uint32_t* map,
			 	     	 uint32_t start,
				     	 uint32_t pages,
			   			 unsigned int type	);

/* As a companion to page_ensure, page_present() will return 0 if  
 * the specified page is missing from the provided map.  */
int		page_present( uint32_t* map, uint32_t address );

/* Provide virtual memory (allocating physical memory) to the
 * map, somewhere between start and end for pages.
 */

uint32_t     page_map_in( uint32_t* map,
			 	     uint32_t start,
			 	     uint32_t end,
				     uint32_t pages,
				     uint32_t type );

/* Release the memory at the virtual address "start" for "pages"
 * amount of pages using the memory map.  This frees the physical
 * memory as well.
 */

int			page_map_out( uint32_t* map, 
						  uint32_t start, 
						  int pages );


/* Map in the physical memory into the map somewhere in the
 * region of start to end
 */

uint32_t     page_provide( uint32_t* map,
			 	     		uint32_t physical,
			 	     		uint32_t start,
			 	     		uint32_t end,
				     		uint32_t pages,
				     		uint32_t type );

/* Map in the physical region of memory directly to start, for pages amount of pages */
int			page_direct_map( uint32_t* map, uint32_t physical, uint32_t start, int pages, unsigned int type );


/* Release the pages at start address in the map but do not free the physical memory */

int			page_release( uint32_t* map, uint32_t start, int pages );


int page_copy_out( uint32_t *map, uint32_t dest, uint32_t src, int bytes );
int page_copy_in(  uint32_t* map, uint32_t dest, uint32_t src, int bytes );


uint32_t page_data_get( uint32_t *cr3, uint32_t address );

#endif


