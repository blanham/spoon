#include <configure.h>
#include <types.h>
#include <conio.h>
#include <dmesg.h>
#include <macros.h>
#include <interrupts.h>



#define MEM_TOTAL_PAGES    ( MAX_PHYSICAL_MEMORY  / 4 )
#define MEM_TOTAL_ELEMENTS ( (MEM_TOTAL_PAGES / 8)  + 1 )

/** Memory is managed by using a bitmap.
 */

static uint8 memory_bitmap[ MEM_TOTAL_ELEMENTS ];			// The memory bitmap.

#define ISSET(block)	\
			(memory_bitmap[ ((block)/8) ] & (uint8)(1<<((block)%8))) != 0

#define SET(block)		\
			memory_bitmap[ ((block)/8) ] |= (uint8)(1<<((block)%8));  \
			memory_freePages -= 1

#define UNSET(block)		\
			memory_bitmap[ ((block)/8) ] &= ~((uint8)(1<<((block)%8))); \
			memory_freePages += 1


// -----------------------------------------------

static int memory_totalPages;		//< The total number of pages available.
static int memory_freePages;		//< The total number of free pages.
static int memory_lastPage;  		//< The last page used in the bitmap.



/** The initialization of memory requires that all memory data
 * structures and variables are intiialized.
 *
 * The lastPage parameter is the last page of the kernel or - rather -
 * the first page that the kernel can start using.
 */
void init_memory( int totalPages )
{
  int i;
  
  dmesg("Initialising memory managment.\n");
  dmesg("Cleaning memory map.\n"); 
  
  // set all blocks as taken
    for (i = 0; i < MEM_TOTAL_ELEMENTS; i++)  memory_bitmap[i] = 0xFF;

	memory_totalPages = (totalPages > MEM_TOTAL_PAGES) ? 
								MEM_TOTAL_PAGES : totalPages;
	memory_freePages  = 0;
	memory_lastPage   = 0;
}


void show_memory_map()
{
	int i;
	int last = 0;
	int taken = 0;

	if ( ISSET(0) ) taken = 1;
		
	for ( i = 0; i < MEM_TOTAL_PAGES; i++ )
	{
		if ( ISSET(i) && ( taken == 0 ) )
		{
			dmesg("Available: %x -- %x\n",
					 (unsigned int)(last * 4096),
					 (unsigned int)(i * 4096 - 1) );
			last = i;
			taken = 1;
		}
		else
			if ( ! (ISSET(i)) && ( taken == 1 ) )
			{
				dmesg("Unavailable: %x -- %x\n",
						 (unsigned int)(last * 4096),
						 (unsigned int)(i * 4096 - 1) );
				last = i;
				taken = 0;
			}
	}
}


/** 
 */
void* memory_alloc( int pages )
{
	void *ptr = NULL;
	int enabled = disable_interrupts();

	ASSERT( pages > 0 );

	// Not enough memory.
	if ( memory_freePages < pages )
	{
		if ( enabled == 1 ) enable_interrupts();
		return NULL;
	}

	ptr = memory_multiple_alloc( pages );
	if ( ptr == NULL )
		dmesg("%!OUT OF MEMORY. RETURNING NULL (%i,%i,%i)\n", 
					memory_totalPages, memory_freePages, pages );
	
	if ( enabled == 1 ) enable_interrupts();
	return ptr;
}


/** Scan the memory bitmap for a contiguous section of memory of pages length. 
 *  Note that use of this function will invalidate some of the memory stack
 *  information.  The stack is disorganized and so, when bits are marked
 *  as set here, you won't know where they are in the stack... better to leave
 *  them and have the memory_alloc check to see their validity. 
 */
void* memory_multiple_alloc( int pages )
{
  int i, j;
  int block = -1;

  int enabled = disable_interrupts();

  for ( i = 0; i < memory_totalPages; i++ )
  {
	int pos = (i + memory_lastPage) % memory_totalPages;
	
	if ( (pos + pages) > memory_totalPages ) 
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
  	if ( block == -1 )
  	{
		if ( enabled == 1 ) enable_interrupts();
		return NULL;
	}
	

	for ( i = 0; i < pages; i++ )
	{
		SET( block + i );
	}

	memory_lastPage = block + pages;
  
  if ( enabled == 1 ) enable_interrupts();
  return (void*)((unsigned int)block * PAGE_SIZE );
}




/** Unmarks the bit in the bitmap and pushes the
 * available page back onto the stack.
 */
void memory_free( int pages, void *position )
{
	int start, index;
	int enabled = disable_interrupts();
		
	ASSERT(  (((uint32)position % PAGE_SIZE) == 0 )  );
	
	start = ((uint32)position) / PAGE_SIZE;
	
	for (index = 0; index < pages; index++)
	{
		int block = start + index;

		ASSERT( ISSET( block )  );
		UNSET( block );
	}
	
	if ( enabled == 1 ) enable_interrupts();
}





// ------------------------------------------------------------------------
//


unsigned int free_memory()  
{  
	return (memory_freePages);  
}

unsigned int total_memory() 
{  
	return (memory_totalPages); 
}




// -----------------------------------


/** Sets a page as used (1) or not used (0). Please use 
 * this function sparingly as it creates more invalid 
 * data in the memory stack.
 */
void memory_internalSet( int block, int used )
{
	ASSERT( (block>=0) && (block<MEM_TOTAL_PAGES) );

	int pos 	= block / 8;
	uint8 value = used << (block % 8);
	uint8 bit   = 1 << ( block % 8 );
		
	if ( (memory_bitmap[ pos ] & bit) == value  ) return;

	if ( used == 1 )
	{
		SET( block );
	}
	else
	{
		UNSET( block );
	}
}






