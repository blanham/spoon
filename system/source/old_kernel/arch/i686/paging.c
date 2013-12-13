#include <configure.h>
#include <macros.h>
#include <dmesg.h>
#include <types.h>
#include <memory.h>
#include <alloc.h>
#include <conio.h>
#include <paging.h>

#include "paging.h"
#include "misc.h"
#include "io.h"

uint32 page_directory[1024] __attribute__ ((aligned (4096)));
                   // keep it aligned on a page boundary
			       // or else we're going to triple fault
			       // when we switch to paging mode.

void init_paging()
{
  unsigned int i;

  dmesg("initializing virtual memory.\n");
  dmesg("building common tables.\n");


  // use 4MB pages to map in the whole 4GB of memory as
  // supervisor read/write memory

  for (i = 0; i < 1024; i ++)
     page_directory[ i ] =   (i* 4096 * 1024) | PAGE_SUPERVISOR
                           | PAGE_4MB | PAGE_RDWR | PAGE_PRESENT | PAGE_GLOBAL;

  
  dmesg("enabling paging mode with PSE (4MB pages).\n"); 

   __asm__ __volatile__
    (
      " \n"
         "mov %0, %%eax;\n"
      
         "mov %%eax, %%cr3;\n"
         "mov %%cr4,%%eax;\n"
         "orl $0x10,%%eax;\n"
         "mov %%eax,%%cr4;\n"

         "mov %%cr0,%%eax;\n"
         "orl $0x80000000,%%eax;\n"
		 "and $0x9FFFFFFF,%%eax;\n"
         "mov %%eax,%%cr0;\n"
      
       :
       :  "g" ( &(page_directory) )
    );
  
  dmesg("paging enabled.\n"); 
}

/*
 
   // These were made into macros and moved into the $ARCH/paging.h header.
   
inline uint32 page_table_of(uint32 address) { return (((address)>>22) & (0x3FF)); }
inline uint32 page_entry_of(uint32 address) { return (((address)>>12) & (0x3FF)); }  
*/


#define page_table( pde, address )	( (uint32*)(pde[ page_table_of(address) ] & ~0xFFF) )
#define page_entry( pde, address )	( (uint32*)(((uint32*)(pde[ page_table_of(address) ] & ~0xFFF) )[page_entry_of(address)] & ~0xFFF) )


inline int page_table_present( uint32 *pde, uint32 address )
{
	uint32 index = page_table_of( address );
	if ( (pde[ index ] & PAGE_PRESENT) != PAGE_PRESENT ) return -1;
	return 0;
}


inline int page_entry_present( uint32 *pde, uint32 address )
{
	uint32 *table;
	uint32 tmp = pde[ page_table_of(address) ];

	if ( (tmp & PAGE_PRESENT) != PAGE_PRESENT ) return -1;

	table = (uint32*)(tmp & ~(0xFFF));
	
	if ( (table[ page_entry_of( address ) ] & PAGE_PRESENT)
			!= PAGE_PRESENT ) return -1;

	return 0;
}




// ********************************************************************


inline uint32 convert_paging_types( unsigned int flags )
{
	uint32 f = 0;

	if ( (flags & READ_ONLY )   != 0 )   f |= PAGE_READ_ONLY;
	if ( (flags & READ_WRITE)   != 0 )   f |= PAGE_RDWR;
	if ( (flags & USER_PAGE )   != 0 )   f |= PAGE_USER;
	if ( (flags & SYSTEM_PAGE ) != 0 )   f |= PAGE_SUPERVISOR;
	if ( (flags & SHARED )	    != 0 )   f |= PAGE_SHARED;

	return f;
}


// ********************************************************************


uintptr     page_ensure( vmmap* map, 
						 uintptr start, 
						 uintptr pages, 
						 unsigned int flags )
{
    uint32 i,j;
	uint32 *directory;
    uint32 *table;
    int32 invalidate;
	uint32 index;

	uint32 type = convert_paging_types( flags );

	directory = map->pde;
    invalidate = 0;

    if ( cr3() == (uint32)directory ) invalidate = 1;

	index = start - PAGE_SIZE;
	
    for ( i = 0; i < pages; i++ )
    {
	  index += PAGE_SIZE;

			
      if ( page_table_present( directory, index ) != 0  )
      {
        table = (uint32*)  memory_alloc(1);
        for (j = 0; j < 1024; j++) table[j] = 0;
        directory[ page_table_of( index ) ] = (uint32)table | type | PAGE_PRESENT;
      }
	    

     if ( page_entry_present( directory, index ) != 0 ) 
     {
        table = page_table( directory, index );
     	table[ page_entry_of(index) ] = (uint32)memory_alloc(1) | type | PAGE_PRESENT;
     	if ( invalidate == 1 ) invlpg( index );
     }

   }


    return 0; 
 }



uintptr     page_map_in( vmmap* map,
			 	     uintptr start,
			 	     uintptr end,
				     uintptr pages,
				     uintptr type )
{
    uint32 i,j;
	uint32* directory;
    uint32 *table;
    uint32 found_start, found;
    uint32 index;

    int32 invalidate;

	directory = (uint32*)(map->pde);
    found = 0;
    found_start = 0;
    invalidate = 0;

    if ( cr3() == (uint32)directory ) invalidate = 1;

    for ( i = start; i < end; i+= PAGE_SIZE)
    {
	  
      if ( page_table_present( directory, i ) != 0 )
      {
        table = (uint32*)  memory_alloc(1);
        for (j = 0; j < 1024; j++) table[j] = 0;
        directory[ page_table_of(i) ] = (uint32)table | type | PAGE_PRESENT;
      }
	    
        if ( page_entry_present(directory, i) != 0 ) 
        {
          found++;
          if ( found_start == 0 ) found_start = i;
        }
        else 
        {
	      found = 0;
	      found_start  = 0 ;
        }

     if ( found == pages ) break;
   }

    if ( found != pages) return 0;
    

    // now map them in..........
	index = found_start - PAGE_SIZE;
    for ( i = 0; i < pages; i++)
    {
	 index += PAGE_SIZE;
	 table = page_table( directory, index );  
     table[ page_entry_of( index ) ] = (uint32)memory_alloc(1) | type | PAGE_PRESENT;
     if ( invalidate == 1 ) invlpg( index  );
    }
 

    return found_start; 
 }



int			page_direct_map( vmmap* map, uintptr physical, uintptr start, int pages, unsigned int type )
{
	unsigned int i,j;
	uint32* directory;
	uint32* table;
	uint32 flags;
	uint32 phys;

	int invalidate = 0;
	
	uint32 index;

	flags = convert_paging_types( type );

	directory = map->pde;

	if ( cr3() == (uint32)(directory) ) invalidate = 1;
	
	index = start;
	phys = physical;
	
 	for ( i = 0; i < pages; i++)
	{

		 if ( page_table_present( directory, index ) != 0 )
		 {
			 table = (uint32*)  memory_alloc(1);
			 for (j = 0; j < 1024; j++) table[j] = 0;
			 directory[ page_table_of( index ) ] = (uint32)table | flags | PAGE_PRESENT;
		 }


	   table = page_table( directory, index );
	   table[ page_entry_of( index ) ] = (phys) | flags |  PAGE_PRESENT;

	   if ( invalidate == 1 ) invlpg( index );

	   phys  += PAGE_SIZE;
       index += PAGE_SIZE;
	}

	return 0;
}


uintptr     page_provide( vmmap* map,
			 	     		uintptr physical,
			 	     		uintptr start,
			 	     		uintptr end,
				     		uintptr pages,
				     		uintptr type )
{
	uintptr location;

	location = page_find_free( map, start, end, pages );
	if ( location == 0 ) return 0;
	if ( page_direct_map( map, physical, location, pages, type ) != 0 ) return 0;

	return location;
}





int			page_release( vmmap* map, uintptr start, int pages )
{
	int i;
	uint32* table;
	uint32* directory = (uint32*)(map->pde);
	uint32 index;
	int invalidate = 0;

	if ( cr3() == (uint32)(directory) ) invalidate = 1;

	index = start;

 	for ( i = 0; i < pages; i++)
	{

	  if ( page_entry_present( directory, index ) == 0 )
	  {
	   table = page_table( directory, index );
	   table[ page_entry_of( index ) ] = 0;
	  }

	  if ( invalidate == 1 ) invlpg( index );
	  index += PAGE_SIZE;
	}

	return 0;
}


int			page_map_out( vmmap* map, uintptr start, int pages )
{
	int i;
	uint32* page;
	uint32* table;
	uint32* directory = (uint32*)(map->pde);
	uint32 index;
	int invalidate = 0;

    if ( cr3() == (uint32)directory ) invalidate = 1;

	index = start;

 	for ( i = 0; i < pages; i++)
	{

	  if ( page_entry_present( directory, index ) == 0 )
	  {
	   table = page_table( directory, index );
	    page = page_entry( directory, index );
	   table[ page_entry_of( index ) ] = 0;
	   memory_free( 1, page );
	  }

	  if ( invalidate == 1 ) invlpg( index );
	  index += PAGE_SIZE;
	}

	return 0;
}





int page_copy_out( vmmap *map, uintptr dest, uintptr src, int bytes )
{
    int i;
    uint32 *table;
    uint32 found;
    char *d, *s;

	uint32* directory = (uint32*)map->pde;

    d = (char*)dest;
    for ( i = 0; i < bytes ; i++)
    {
        table = page_table( directory, src + i );
	    found = (uint32)(page_entry( directory, src + i ));
  	    found = found + ( src + i ) % PAGE_SIZE;
	    s = (char*)found;
	    d[i] = s[0];
    }
    return bytes; 
}

int page_copy_in( vmmap* map,  uintptr dest, uintptr src, int bytes )
{
    int i;
    uint32 *table;
    uint32 found;
    char *d, *s;

	uint32* directory = map->pde;

    s = (char*)src;
    for ( i = 0; i < bytes; i ++ )
    {
      table = page_table( directory, dest + i );
  	  found = (uint32)(page_entry( directory, dest + i ));
	  found = found +  ( dest + i ) % PAGE_SIZE;
	  d = (char*)found;
	  d[0] = s[i];
    }
    return bytes; 
}






uintptr page_find_free( vmmap* map, 
					  uintptr start,
					  uintptr finish,
					  uintptr pages )
{
	uint32 *directory;
	uintptr address;
	uintptr current;
	uintptr found;
	uintptr count;

	directory = (uint32*)map->pde;

	found = 0;
	count = 0;
	address = (start & ~0xFFF);


   for ( current = address; current < finish; current = current + PAGE_SIZE )
	   {
	      found = 1;
	      for ( count = 0; count < pages; count++)
	        if ( page_entry_present( directory, current + count * PAGE_SIZE ) == 0 )
	        {
	   	     found = 0;
			 current = current + count * PAGE_SIZE;
		     break;
   	        }

	   
	      if ( found == 1 )
	      {
 		   found = current;
		   break;
	      }
		  
	      found = 0;
	   }
	

	return found;
}

// --------------------------------------

uint32 page_data_get( uint32 *cr3, uint32 address )
{
	uint32 *table;
	uint32 page;

	if ( (cr3[ page_table_of( address ) ] & PAGE_PRESENT) != PAGE_PRESENT ) return -1;

	// 4 MB ---------
	if ( (cr3[ page_table_of( address ) ] & PAGE_4MB ) != 0)
	{
		page = (cr3[ page_table_of( address)] & ~( 1024 *  4096 - 1));
		return *((uint32*)(page + (address & ( 1024 * 4096 - 1 )))) ;
	}

	// 4 KB -------------------
	table = (uint32*)(cr3[ page_table_of( address ) ] & ~0xFFF);
	
	if ( (table[ page_entry_of( address ) ] & PAGE_PRESENT) != PAGE_PRESENT ) return -1;
	
	table = (uint32*)(table[page_entry_of( address )] & ~0xFFF);
	
	return *((uint32*)((uint32)table + (address & ( PAGE_SIZE - 1)))) ;
}

								


// --------------------------------------

vmmap* new_vmmap()
{
  unsigned int i;
  uint32 *pde;
  vmmap *map;

  map = malloc( sizeof( vmmap ) );
  pde = (uint32*)( memory_alloc( 1) );
  
  map->pde = pde;
  
  for (i = 0; i < 512; i++)
     pde[ i ] =   (i* 4096 * 1024) | 
	     		   PAGE_SUPERVISOR | 
				   PAGE_4MB | 
				   PAGE_RDWR | 
				   PAGE_PRESENT | 
				   PAGE_GLOBAL;

  for ( i = 512; i < 1024; i++ ) pde[ i ] = 0;
  
  return map;
}

int    delete_vmmap( vmmap* map )
{
	memory_free( 1, map->pde );
	free( map );
	return 0;
}




int   set_vmmap( vmmap*  map )
{
 __asm__ ( "  mov %0, %%eax\n"
	         "mov %%eax, %%cr3 "
		    : 
		    : "g" ((uint32)(map->pde))
		    : "eax"
		    );

  return 0;
}

int   get_vmmap( vmmap* map )
{
	uint32 cr3;
	
	__asm__ ( "mov %%cr3, %%eax\n"
	          "mov %%eax, %0 "
		    : "=g" (cr3)
		    : 
		    : "eax"
		    );

	map->pde = (uint32*)cr3;
	
	return 0;
}











