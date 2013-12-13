#ifndef _KERNEL_i686_PAGING_H
#define _KERNEL_i686_PAGING_H

#include <types.h>


// ------------------- PAGE CONSTANTS

//               ----- DEFINES

#define PAGE_PRESENT			(1)
#define PAGE_WRITE_THROUGH		(1<<3)
#define PAGE_WRITE_BACK			0x0
#define PAGE_CACHE_ENABLE		0x0
#define PAGE_CACHE_DISABLE		(1<<4)
#define PAGE_READ_ONLY			0x0
#define PAGE_RDWR			(1<<1)
#define PAGE_USER			(1<<2)
#define PAGE_SUPERVISOR			0x0
#define PAGE_GLOBAL			(1<<8)
#define PAGE_DIRTY			0x40 // only for PTE
#define PAGE_ACCESSED			(1<<5)
#define PAGE_LOCAL			0x0
#define PAGE_4MB			(1<<7) // only for PDE's
#define PAGE_4KB			0x0  // only for PDE'S

// ----------- USE OF AVAIL SECTION


#define PAGE_SHARED			(1<<9)



void init_paging();


extern uint32 page_directory[1024];


#define page_table_of(address)  (((address)>>22) & 0x3FF) 
#define page_entry_of(address)  (((address)>>12) & 0x3FF) 


uint32 page_data_get( uint32 *cr3, uint32 address );

#endif


