#include <inttypes.h>
#include <atomic.h>
#include <assert.h>
#include <multiboot.h>
#include <interrupts.h>
#include <dmesg.h>


// --------- ACTUAL PHYSICAL MEMORY BITMAP ----------------------
// Please go ALL THE WAY DOWN for the initialization routines.

// Basically 2 GB worth of pages. (2 GB / 4096 => 2^19)
// And 2^14 uint32_t required to manage that.

#define TOTAL_PAGES    ( 2<<19 )
#define MAXMAP			( 2<<14 )

static uint32_t memory_bitmap[ MAXMAP ];

#define ISSET(block)	\
			((memory_bitmap[ ((block)/32) ] & (uint32_t)(1<<((block)%32))) != 0)

#define SET(block)		\
			memory_bitmap[ ((block)/32) ] |= (uint32_t)(1<<((block)%32));  \
			s_freePages -= 1

#define UNSET(block)		\
			memory_bitmap[ ((block)/32) ] &= ~((uint32_t)(1<<((block)%32))); \
			s_freePages += 1


// -----------------------------------------------

static int s_totalPages;	//< The total number of pages available.
static int s_freePages;		//< The total number of free pages.
static int s_lastPage;  	//< The last page used in the bitmap.



void show_memory_map()
{
	int i = 0;
	int last = 0;
	int taken = 0;

	if ( ISSET(0) ) taken = 1;
	
	dmesg("Physical memory summary:\n");
	
	for ( i = 0; i < TOTAL_PAGES; i++ )
	{
		if ( ISSET(i) && ( taken == 0 ) )
		{
			dmesg("  Available: %x -- %x\n",
					 (unsigned int)(last * 4096),
					 (unsigned int)(i * 4096 - 1) );
			last = i;
			taken = 1;
		}
		else
			if ( ! (ISSET(i)) && ( taken == 1 ) )
			{
				dmesg("  Unavailable: %x -- %x\n",
						 (unsigned int)(last * 4096),
						 (unsigned int)(i * 4096 - 1) );
				last = i;
				taken = 0;
			}

	}

	if ( taken == 0 )
			dmesg("  Available: %x -- %x\n",
					 (unsigned int)(last * 4096),
					 (unsigned int)((uint32_t)TOTAL_PAGES * 4096 - 1) );
	else
			dmesg("  Unavailable: %x -- %x\n",
					 (unsigned int)(last * 4096),
					 (unsigned int)((uint32_t)TOTAL_PAGES * 4096 - 1) );
	
}




void* memory_multiple_alloc( int pages )
{
	int i, j;
	int block = -1;

	for ( i = 0; i < MAXMAP; i++ )
	{
		int pos = (i + s_lastPage) % MAXMAP;
		if ( (pos + pages) > MAXMAP ) 
		{
			i += pages - 2;
			continue;	
		}
				// Contiguous does not mean wrapped-around.

				// Scan from the current pos up for pages amount.
		block = pos;
		for ( j = 0; j < pages; j++ )
		{
			if ( ISSET( pos + j ) ) 
			{
				block = -1;
				i = i + j;		// Don't waste time.
				break;
			}
		}

		if ( block != -1 ) break;	// Hooray. We have our range.
	}

	// Nothing was found..
	if ( block == -1 ) return NULL;
	

	for ( i = 0; i < pages; i++ )
	SET( block + i );

	s_lastPage = block + pages;
  
	return (void*)((unsigned int)block * PAGE_SIZE );
}



void* memory_alloc( int pages )
{
	void *ptr = NULL;
	assert( pages > 0 );

	begin_critical_section();

		// Not enough memory.
		if ( s_freePages > pages )
		{
			ptr = memory_multiple_alloc( pages );
			if ( ptr == NULL )
				dmesg("%!OUT OF MEMORY. RETURNING NULL (%i,%i,%i)\n", 
							s_totalPages, s_freePages, pages );
		}

	end_critical_section();
	return ptr;
}



/** Unmarks the bit in the bitmap and pushes the
 * available page back onto the stack.
 */
void memory_free( int pages, void *position )
{
	int start, index;

	begin_critical_section();
		
		assert(  (((uintptr_t)position % PAGE_SIZE) == 0 )  );
		start = ((uintptr_t)position) / PAGE_SIZE;
		
		for (index = 0; index < pages; index++)
		{
			int block = start + index;
			assert( ISSET( block )  );
			UNSET( block );
		}
	
	end_critical_section();
}







// ********** INITIALIZATION ROUTINES ********************


static uint64_t parse_multibootMap( memory_map_t* m_block, unsigned int count)
{
  uint64_t mem_total = 0;
  uintptr_t page_begin, page_end;
  int i;
  
  
  dmesg( "BIOS Memory Map:\n" );

  for (i = 0; i < count; i ++)
  {
	uint64_t base_addr = m_block[i].base_addr_high;
			 base_addr = (base_addr << 32) + m_block[i].base_addr_low;
	
	uint64_t base_length = m_block[i].length_high;
			 base_length = (base_length << 32) + m_block[i].length_low;
		  

	dmesg( "  (%x) - (%x) ",
					(unsigned int)base_addr,
					(unsigned int)(base_addr + base_length ) );
    
	switch( m_block[i].type )
	{
		case 1: dmesg("<available>\n");  	break;
		case 2: dmesg("<reserved>\n");		break;
		case 3:	dmesg("<acpi reclaim>\n"); break;
		case 4: dmesg("<acpi nvs>\n"); 	break;
		default: dmesg("<dunno>\n"); break;
	}
	


       if (m_block[i].type == 0x1)
       {
			mem_total += base_length;

           page_begin = base_addr / PAGE_SIZE;

           if ( (base_addr % PAGE_SIZE) != 0) page_begin += 1;  
		   				// we only support full pages

           page_end = (base_addr + base_length) / PAGE_SIZE;

           if ( ((base_addr + base_length) % PAGE_SIZE) == 0 ) 
				   		page_end -= 1;

		   // Mark as free.
			   while ( (page_begin <= page_end) && (page_begin < MAXMAP) )
			   {
				UNSET( (unsigned int)page_begin );
				s_totalPages += 1;
				page_begin += 1;
			   }
       }
    
  }
  
  return mem_total;
}


extern uintptr_t _KERNEL_START;
extern uintptr_t _KERNEL_END;
static uintptr_t KERNEL_START = (uintptr_t)&(_KERNEL_START);
static uintptr_t KERNEL_END = (uintptr_t)&(_KERNEL_END);


void init_memory( multiboot_info_t *mboot )
{
  int i;
  unsigned int count = (mboot->mmap_length / sizeof(memory_map_t) );
  memory_map_t *addr = (memory_map_t*)mboot->mmap_addr;

	    for (i = 0; i < MAXMAP; i++)  memory_bitmap[i] = 0xFFFFFFFF;
		s_totalPages = 0; 
		s_freePages  = 0;
		s_lastPage   = 0;
  
  parse_multibootMap( addr, count );

  dmesg(" Reserving first megabyte of memory.\n"); 
  for (i = 0; i < 256; i++) SET( i );

  dmesg("Reserving kernel memory: %x - %x\n", KERNEL_START, KERNEL_END ); 
  for (i = (KERNEL_START / PAGE_SIZE); i < (KERNEL_END / PAGE_SIZE); i++)  
		 SET( i );

  
  show_memory_map();
}


/** Note that start and end are real addresses and not page numbers */
void internal_reserve( uint32_t start, uint32_t end, int used )
{
  int i;
  int endCheck =  ((end % PAGE_SIZE) != 0 ) ? 1 : 0;
		
  for (i = (start / PAGE_SIZE); i < (end / PAGE_SIZE + endCheck); i++)  
  {
	if ( used == 1 ) 
	{
		SET( i );
	}
	else
	{
		UNSET( i );
	}
  }
}



