#include <configure.h>
#include <types.h>
#include <conio.h>
#include <dmesg.h>
#include <macros.h>
#include <interrupts.h>
#include <memory.h>

#include "multiboot.h"


extern uint32 _KERNEL_START;
extern uint32 _KERNEL_END;

static uint32 KERNEL_START = (uint32)&_KERNEL_START;
static uint32 KERNEL_END   = (uint32)&_KERNEL_END;


uint32 use_bios_memory_map(uint32 addr, uint32 count);
int use_bios_totalMemory(uint32 addr, uint32 count);

void arch_init_memory(multiboot_info_t *mboot)
{
  uint32 i;
  uint32 addr;
  uint32 count;
  int totalPages;


  addr = mboot->mmap_addr;
  count = (mboot->mmap_length / sizeof(memory_map_t) );
  
  totalPages = use_bios_totalMemory( addr, count );

  dmesg(" There are %i total pages = %i KB\n",
				  totalPages,
				  totalPages * PAGE_SIZE );

  // --------*****************************************************************
  init_memory( totalPages );
  use_bios_memory_map(addr,count);
  // --------*****************************************************************
  

  dmesg(" Reserving first megabyte of memory.\n"); 
  for (i = 0; i < 256; i++) 
    			memory_internalSet( i, 1 );

  dmesg(" Reserving multiboot memory.\n"); 
  memory_internalSet( (uint32)(mboot) / PAGE_SIZE, 1 );
 

  dmesg("%s%i - %i\n",  " Reserving kernel memory: "
					  	   ,  KERNEL_START
						   ,  KERNEL_END  ); 
  
  for (i = (KERNEL_START / PAGE_SIZE); i < (KERNEL_END / PAGE_SIZE  + 1); i++)  
	memory_internalSet( i, 1 );

  show_memory_map();
}



// ------------------- MEMORY DETECTION ROUTINES ---------------------------

int use_bios_totalMemory(uint32 addr, uint32 count)
{
  memory_map_t  *mblock;

  int total_pages = 0;
  int i = 0;
  unsigned int page_begin, page_end;
  
  
  for (i = 0; i < count; i++)
  {
    mblock = (memory_map_t*)( addr + i * sizeof(memory_map_t) );
    if (mblock->base_addr_high != 0) break; // Too high for me.
    

       if (mblock->type == 0x1)
       {
           page_begin = mblock->base_addr_low / PAGE_SIZE;
           page_end = (mblock->base_addr_low + mblock->length_low) / PAGE_SIZE;

           if ( (mblock->base_addr_low % PAGE_SIZE) != 0) page_begin++;

		   total_pages += (page_end - page_begin);
       }
    
  }
  
  return total_pages;
}








uint32 use_bios_memory_map(uint32 addr, uint32 count)
{
  memory_map_t  *mblock;

  uint64 mem_total = 0;
  int i = 0;
  unsigned int page_begin, page_end, page_index;
  
  
  dmesg( "          : BIOS Memory Map:\n" );

  for (i = 0; i < count; i ++)
  {
    mblock = (memory_map_t*)( addr + i * sizeof(memory_map_t) );

    if (mblock->base_addr_high != 0) break;

	dmesg( "%s%x%s%x%s","     (",mblock->base_addr_low,") - (",mblock->base_addr_low + mblock->length_low,")  ");
    
	switch( mblock->type )
	{
		case 1: dmesg("%s\n","<available>");  	break;
		case 2: dmesg("%s\n","<reserved>");		break;
		case 3:	dmesg("%s\n","<acpi reclaim>"); break;
		case 4: dmesg("%s\n","<acpi nvs>"); 	break;
		default:
				dmesg("%s\n","<dunno>");
				break;
	}
	

       if ( mblock->type == 0x1 ) mem_total = mem_total + mblock->length_low / 1024;

       if (mblock->type == 0x1)
       {
           page_begin = mblock->base_addr_low / PAGE_SIZE;
           if ( (mblock->base_addr_low % PAGE_SIZE) != 0) page_begin++;  // we only support full pages
           page_end = (mblock->base_addr_low + mblock->length_low) / PAGE_SIZE;
           if ( ((mblock->base_addr_low + mblock->length_low) % PAGE_SIZE) != 0 ) page_end--;


           for ( page_index = page_begin; page_index < page_end; page_index++)  
				   memory_internalSet( page_index, 0 );
       }
    
  }
  
  if (mem_total > MAX_PHYSICAL_MEMORY) mem_total = MAX_PHYSICAL_MEMORY;  // is it too big?  
  return (uint32)mem_total;
}








