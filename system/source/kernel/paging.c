#include <inttypes.h>
#include <dmesg.h>
#include <paging.h>
#include <physmem.h>
#include <misc.h>


uint32_t page_directory[1024] 
			__attribute__ ((aligned (4096)));
                   // aligned to prevent triple-fault.

void init_paging()
{
  unsigned int i;

  // 4MB pages over 4GB, supervisor, RW

  for (i = 0; i < 1024; i ++)
     page_directory[ i ] =   (i * 4096 * 1024) | PAGE_SUPERVISOR
           	              | PAGE_4MB | PAGE_RDWR | PAGE_PRESENT | PAGE_GLOBAL;

}


void load_paging( unsigned int cpu_id )
{
   asm volatile
    (
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
  
  dmesg("CPU %i paging enabled.\n", cpu_id); 
}



uint32_t* new_page_directory()
{
	int i;
	uint32_t *map = memory_alloc( 1 );


  	for (i = 0; i < 512; i ++)
     map[ i ] =   (i * 4096 * 1024) | PAGE_SUPERVISOR
           	          | PAGE_4MB | PAGE_RDWR | PAGE_PRESENT | PAGE_GLOBAL;

  	for (i = 512; i < 1024; i ++)
     map[ i ] =   0;


  	for (i = 1016; i < 1024; i ++)
     map[ i ] =   (i * 4096 * 1024) | PAGE_SUPERVISOR
           	          | PAGE_4MB | PAGE_RDWR | PAGE_PRESENT | PAGE_GLOBAL;

	return map;
}


void 	  delete_page_directory( uint32_t* map )
{
#warning implement
}




#define page_table_of(address)  (((address)>>22) & 0x3FF) 
#define page_entry_of(address)  (((address)>>12) & 0x3FF) 

#define page_table( pde, address )	( (uint32_t*)(pde[ page_table_of(address) ] & ~0xFFF) )
#define page_entry( pde, address )	( (uint32_t*)(((uint32_t*)(pde[ page_table_of(address) ] & ~0xFFF) )[page_entry_of(address)] & ~0xFFF) )


inline int page_table_present( uint32_t *pde, uint32_t address )
{
	uint32_t index = page_table_of( address );
	if ( (pde[ index ] & PAGE_PRESENT) != PAGE_PRESENT ) return -1;
	return 0;
}


inline int page_entry_present( uint32_t *pde, uint32_t address )
{
	uint32_t *table;
	uint32_t tmp = pde[ page_table_of(address) ];

	if ( (tmp & PAGE_PRESENT) != PAGE_PRESENT ) return -1;

	table = (uint32_t*)(tmp & ~(0xFFF));
	
	if ( (table[ page_entry_of( address ) ] & PAGE_PRESENT)
			!= PAGE_PRESENT ) return -1;

	return 0;
}




// ********************************************************************


inline uint32_t convert_paging_types( unsigned int flags )
{
	uint32_t f = 0;

	if ( (flags & READ_ONLY )   != 0 )   f |= PAGE_READ_ONLY;
	if ( (flags & READ_WRITE)   != 0 )   f |= PAGE_RDWR;
	if ( (flags & USER_PAGE )   != 0 )   f |= PAGE_USER;
	if ( (flags & SYSTEM_PAGE ) != 0 )   f |= PAGE_SUPERVISOR;

	return f;
}


// ********************************************************************


/** Returns the number of pages which were already mapped in. Ideally,
 * you should assert on this returning non-zero. */
int     page_ensure( uint32_t* directory, 
						 uint32_t start, 
						 uint32_t pages, 
						 unsigned int flags )
{
    uint32_t i,j;
    uint32_t *table;
    int invalidate;
	uint32_t index;
	int alreadies = 0;

	uint32_t type = convert_paging_types( flags );

    invalidate = 0;

    if ( cr3() == (uint32_t)directory ) invalidate = 1;

	index = start - PAGE_SIZE;
	
    for ( i = 0; i < pages; i++ )
    {
	  index += PAGE_SIZE;

			
      if ( page_table_present( directory, index ) != 0  )
      {
        table = (uint32_t*)  memory_alloc(1);
        for (j = 0; j < 1024; j++) table[j] = 0;
        directory[ page_table_of( index ) ] = (uint32_t)table | type | PAGE_PRESENT;
      }
	    

     if ( page_entry_present( directory, index ) != 0 ) 
     {
        table = page_table( directory, index );
     	table[ page_entry_of(index) ] = (uint32_t)memory_alloc(1) | type | PAGE_PRESENT;
     	if ( invalidate == 1 ) invlpg( index );
     }
	 else
			alreadies += 1;

   }


    return alreadies; 
 }


/** Returns 0 if the page is present. */
int		page_present( uint32_t* directory, uint32_t address )
{
	if ( page_table_present( directory, address ) != 0 ) return -1;
	if ( page_entry_present( directory, address ) != 0 ) return -1; 
	return 0;
}


uint32_t     page_map_in( uint32_t* directory,
			 	     uint32_t start,
			 	     uint32_t end,
				     uint32_t pages,
				     uint32_t type )
{
    uint32_t i,j;
    uint32_t *table;
    uint32_t found_start, found;
    uint32_t index;

    int invalidate;

    found = 0;
    found_start = 0;
    invalidate = 0;

    if ( cr3() == (uint32_t)directory ) invalidate = 1;

    for ( i = start; i < end; i+= PAGE_SIZE)
    {
	  
      if ( page_table_present( directory, i ) != 0 )
      {
        table = (uint32_t*)  memory_alloc(1);
        for (j = 0; j < 1024; j++) table[j] = 0;
        directory[ page_table_of(i) ] = (uint32_t)table | type | PAGE_PRESENT;
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
     table[ page_entry_of( index ) ] = (uint32_t)memory_alloc(1) | type | PAGE_PRESENT;
     if ( invalidate == 1 ) invlpg( index  );
    }
 

    return found_start; 
 }



int			page_direct_map( uint32_t* directory, uint32_t physical, uint32_t start, int pages, unsigned int type )
{
	unsigned int i,j;
	uint32_t* table;
	uint32_t flags;
	uint32_t phys;

	int invalidate = 0;
	
	uint32_t index;

	flags = convert_paging_types( type );

	if ( cr3() == (uint32_t)(directory) ) invalidate = 1;
	
	index = start;
	phys = physical;
	
 	for ( i = 0; i < pages; i++)
	{

		 if ( page_table_present( directory, index ) != 0 )
		 {
			 table = (uint32_t*)  memory_alloc(1);
			 for (j = 0; j < 1024; j++) table[j] = 0;
			 directory[ page_table_of( index ) ] = (uint32_t)table | flags | PAGE_PRESENT;
		 }


	   table = page_table( directory, index );
	   table[ page_entry_of( index ) ] = (phys) | flags |  PAGE_PRESENT;

	   if ( invalidate == 1 ) invlpg( index );

	   phys  += PAGE_SIZE;
       index += PAGE_SIZE;
	}

	return 0;
}


uint32_t     page_provide( uint32_t* directory,
			 	     		uint32_t physical,
			 	     		uint32_t start,
			 	     		uint32_t end,
				     		uint32_t pages,
				     		uint32_t type )
{
	uint32_t location;

	location = page_find_free( directory, start, end, pages );
	if ( location == 0 ) return 0;
	if ( page_direct_map( directory, physical, location, pages, type ) != 0 ) return 0;

	return location;
}





int			page_release( uint32_t* directory, uint32_t start, int pages )
{
	int i;
	uint32_t* table;
	uint32_t index;
	int invalidate = 0;

	if ( cr3() == (uint32_t)(directory) ) invalidate = 1;

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


int			page_map_out( uint32_t* directory, uint32_t start, int pages )
{
	int i;
	uint32_t* page;
	uint32_t* table;
	uint32_t index;
	int invalidate = 0;

    if ( cr3() == (uint32_t)directory ) invalidate = 1;

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





int page_copy_out( uint32_t* directory, uint32_t dest, uint32_t src, int bytes )
{
    int i;
    uint32_t *table;
    uint32_t found;
    char *d, *s;

    d = (char*)dest;
    for ( i = 0; i < bytes ; i++)
    {
        table = page_table( directory, src + i );
	    found = (uint32_t)(page_entry( directory, src + i ));
  	    found = found + ( src + i ) % PAGE_SIZE;
	    s = (char*)found;
	    d[i] = s[0];
    }
    return bytes; 
}

int page_copy_in( uint32_t* directory,  uint32_t dest, uint32_t src, int bytes )
{
    int i;
    uint32_t *table;
    uint32_t found;
    char *d, *s;

    s = (char*)src;
    for ( i = 0; i < bytes; i ++ )
    {
      table = page_table( directory, dest + i );
  	  found = (uint32_t)(page_entry( directory, dest + i ));
	  found = found +  ( dest + i ) % PAGE_SIZE;
	  d = (char*)found;
	  d[0] = s[i];
    }
    return bytes; 
}






uint32_t page_find_free( uint32_t* directory, 
					  uint32_t start,
					  uint32_t finish,
					  uint32_t pages )
{
	uint32_t address;
	uint32_t current;
	uint32_t found;
	uint32_t count;

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

uint32_t page_data_get( uint32_t *cr3, uint32_t address )
{
	uint32_t *table;
	uint32_t page;

	if ( (cr3[ page_table_of( address ) ] & PAGE_PRESENT) != PAGE_PRESENT ) return -1;

	// 4 MB ---------
	if ( (cr3[ page_table_of( address ) ] & PAGE_4MB ) != 0)
	{
		page = (cr3[ page_table_of( address)] & ~( 1024 *  4096 - 1));
		return *((uint32_t*)(page + (address & ( 1024 * 4096 - 1 )))) ;
	}

	// 4 KB -------------------
	table = (uint32_t*)(cr3[ page_table_of( address ) ] & ~0xFFF);
	
	if ( (table[ page_entry_of( address ) ] & PAGE_PRESENT) != PAGE_PRESENT ) return -1;
	
	table = (uint32_t*)(table[page_entry_of( address )] & ~0xFFF);
	
	return *((uint32_t*)((uint32_t)table + (address & ( PAGE_SIZE - 1)))) ;
}

								



int   set_map( uint32_t*  map )
{
 __asm__ ( "  mov %0, %%eax\n"
	         "mov %%eax, %%cr3 "
		    : 
		    : "g" (map)
		    : "eax"
		    );

  return 0;
}

uint32_t*   get_map()
{
	uint32_t cr3;
	
	__asm__ ( "mov %%cr3, %%eax\n"
	          "mov %%eax, %0 "
		    : "=g" (cr3)
		    : 
		    : "eax"
		    );

	return (uint32_t*)cr3;
}





