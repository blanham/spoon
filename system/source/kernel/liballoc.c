#include <liballoc.h>

/**  Durand's Super Duper Memory functions.  */


#define LIBALLOC_MAGIC	0xc001c0de


/** A structure found at the top of all system allocated 
 * memory blocks. It details the usage of the memory block.
 */
struct liballoc_major
{
	struct liballoc_major *prev;		//< Linked list information.
	struct liballoc_major *next;		//< Linked list information.
	unsigned int pages;					//< The number of pages in the block.
	unsigned int usage;					//< The number of bytes used in the block.
	struct liballoc_minor *first;		//< A pointer to the first allocated memory in the block.	
};


/** This is a structure found at the beginning of all
 * sections in a major block which were allocated by a
 * malloc, calloc, realloc call.
 */
struct	liballoc_minor
{
	struct liballoc_minor *prev;		//< Linked list information.
	struct liballoc_minor *next;		//< Linked list information.
	struct liballoc_major *block;		//< The owning block. A pointer to the major structure.
	unsigned int magic;					//< A magic number to idenfity correctness.
	unsigned int size; 					//< The size of the memory allocated. Could be 1 byte or more.
};


static struct liballoc_major *l_memRoot = NULL;	//< The root memory block acquired from the system.

static int l_pageSize  = 4096;			//< The size of an individual page. Set up in liballoc_init.
static int l_pageCount = 16;			//< The number of pages to request per chunk. Set up in liballoc_init.



// ***********   HELPER FUNCTIONS  *******************************
static void* 	liballoc_memset(void* s, int c, size_t n)
{
	int i;
	for ( i = 0; i < n ; i++)
		((char*)s)[i] = c;
	
	return s;
}

static void* 	liballoc_memcpy(void* s1, const void* s2, size_t n)
{
  char *cdest;
  char *csrc;
  unsigned int *ldest = (unsigned int*)s1;
  unsigned int *lsrc  = (unsigned int*)s2;

  while ( n >= sizeof(unsigned int) )
  {
      *ldest++ = *lsrc++;
	  n -= sizeof(unsigned int);
  }

  cdest = (char*)ldest;
  csrc  = (char*)lsrc;
  
  while ( n > 0 )
  {
      *cdest++ = *csrc++;
	  n -= 1;
  }
  
  return s1;
}

 


// ***************************************************************



int liballoc_shutdown()
{
	struct liballoc_major *maj;
	struct liballoc_major *tmp;

	maj = l_memRoot;

	while ( maj != NULL )
	{
		tmp = maj->next;
		if ( liballoc_free( maj, maj->pages * l_pageSize) != 0 ) 
				if ( liballoc_abort != NULL ) liballoc_abort( 1 );
		maj = tmp;
	}

	return 0;
}





// --------------------------------------------

static struct liballoc_major *allocate_new_page( unsigned int size )
{
	unsigned int st;
	struct liballoc_major *maj;

		// This is how much space is required.
		st  = size + sizeof(struct liballoc_major);
		st += sizeof(struct liballoc_minor);

				// Perfect amount of space?
		if ( (st % l_pageSize) == 0 )
			st  = st / (l_pageSize);
		else
			st  = st / (l_pageSize) + 1;
							// No, add the buffer. 

		
		// Make sure it's >= the minimum size.
		if ( st < l_pageCount ) st = l_pageCount;
		
		maj = (struct liballoc_major*)liballoc_alloc( st );

		if ( maj == NULL ) return NULL;	// uh oh, we ran out of memory.
		
		maj->prev = NULL;
		maj->next = NULL;
		maj->pages = st;
		maj->usage = sizeof(struct liballoc_major);
		maj->first = NULL;

      return maj;
}



void *malloc(size_t size)
{
	void *p = NULL;
	unsigned int diff;
	struct liballoc_major *maj;
	struct liballoc_minor *min;
	struct liballoc_minor *new_min;


	if ( liballoc_lock != NULL ) liballoc_lock();

	if ( l_memRoot == NULL )
	{
		// This is the first time we are being used.
		l_memRoot = allocate_new_page( size );
		if ( l_memRoot == NULL )
		{
		  if ( liballoc_unlock != NULL ) liballoc_unlock();
		  return NULL;
		}
	}

	// Now we need to bounce through every major and find enough space....

	maj = l_memRoot;

	while ( maj != NULL )
	{
		diff  = maj->pages * (l_pageSize) - maj->usage;	
										// free memory in the block

			
		// CASE 1:  There is not enough space in this major block.
		if ( diff < (size + sizeof( struct liballoc_minor )) )
		{
				// Another major block next to this one?
			if ( maj->next != NULL ) 
			{
				maj = maj->next;		// Hop to that one.
				continue;
			}

			// Create a new major block next to this one and...
			maj->next = allocate_new_page( size );	// next one will be okay.
			if ( maj->next == NULL ) break;			// no more memory.
			maj->next->prev = maj;
			maj = maj->next;
			// .. fall through to CASE 2 ..
		}
		
		// CASE 2: It's a brand new block.
		if ( maj->first == NULL )
		{
			maj->first = (struct liballoc_minor*)(  (unsigned int)maj + sizeof(struct liballoc_major) );
			maj->first->magic 	= LIBALLOC_MAGIC;
			maj->first->prev 	= NULL;
			maj->first->next 	= NULL;
			maj->first->block 	= maj;
			maj->first->size 	= size;
			maj->usage 			+= size + sizeof( struct liballoc_minor );

			p = (void*)((unsigned int)(maj->first) + sizeof( struct liballoc_minor ));

			
			if ( liballoc_unlock != NULL ) liballoc_unlock();		// release the lock
			return p;
		}
				

		// CASE 3: Block in use and enough space at the start of the block.
		diff = (unsigned int)(maj->first) - (unsigned int)maj - sizeof(struct liballoc_major);

		if ( diff >= (size + sizeof(struct liballoc_minor)) )
		{
			// Yes, space in front. Squeeze in.
			maj->first->prev = (struct liballoc_minor*)(  (unsigned int)maj + sizeof(struct liballoc_major) );
			maj->first->prev->next = maj->first;
			maj->first = maj->first->prev;
				
			maj->first->magic 	= LIBALLOC_MAGIC;
			maj->first->prev 	= NULL;
			maj->first->block 	= maj;
			maj->first->size 	= size;
			maj->usage 			+= size + sizeof( struct liballoc_minor );

			p = (void*)((unsigned int)(maj->first) + sizeof( struct liballoc_minor ));
			if ( liballoc_unlock != NULL ) liballoc_unlock();		// release the lock
			return p;
		}
		

		
		// CASE 4: There is enough space in this block. But is it contiguous?
		min = maj->first;
		
			// Looping within the block now...
		while ( min != NULL )
		{
				// CASE 4.1: End of minors in a block. Space from last and end?
				if ( min->next == NULL )
				{
					// the rest of this block is free...  is it big enough?
					diff = (unsigned int)(maj) + (l_pageSize) * maj->pages;
					diff -= ((unsigned int) min + sizeof( struct liballoc_minor ) + min->size); 
						// minus already existing usage..

					if ( diff >= size + sizeof( struct liballoc_minor ) )
					{
						// yay....
						min->next = (struct liballoc_minor*)((unsigned int)min + sizeof( struct liballoc_minor ) + min->size);
						min->next->prev = min;
						min = min->next;
						min->next = NULL;
						min->magic = LIBALLOC_MAGIC;
						min->block = maj;
						min->size = size;
						maj->usage += size + sizeof( struct liballoc_minor );
						p = (void*)((unsigned int)min + sizeof( struct liballoc_minor ));
						if ( liballoc_unlock != NULL ) liballoc_unlock();		// release the lock
						return p;
					}
				}



				// CASE 4.2: Is there space between two minors?
				if ( min->next != NULL )
				{
					// is the difference between here and next big enough?
					diff =  (unsigned int)(min->next) - (unsigned int)min  - sizeof( struct liballoc_minor ) - min->size;
										// minus our existing usage.

					if ( diff >= size + sizeof( struct liballoc_minor ) )
					{
						// yay......
						new_min = (struct liballoc_minor*)((unsigned int)min + sizeof( struct liballoc_minor ) + min->size);

						
						new_min->magic = LIBALLOC_MAGIC;
						new_min->next = min->next;
						new_min->prev = min;
						new_min->size = size;
						new_min->block = maj;
						min->next->prev = new_min;
						min->next = new_min;
						maj->usage += size + sizeof( struct liballoc_minor );
						p = (void*)((unsigned int)new_min + sizeof( struct liballoc_minor ));
						if ( liballoc_unlock != NULL ) liballoc_unlock();		// release the lock
						return p;
					}
				}	// min->next != NULL

				min = min->next;
		} // while min != NULL ...


		// CASE 5: Block full! Ensure next block and loop.
		if ( maj->next == NULL ) 
		{
			// we've run out. we need more...
			maj->next = allocate_new_page( size );		// next one guaranteed to be okay
			if ( maj->next == NULL ) break;			//  uh oh,  no more memory.....
			maj->next->prev = maj;
		}

		maj = maj->next;
	} // while (maj != NULL)

	
	if ( liballoc_unlock != NULL ) liballoc_unlock();		// release the lock
	return NULL;
}





void free(void *ptr)
{
	struct liballoc_minor *min;
	struct liballoc_major *maj;

	if ( ptr == NULL ) return;


	if ( liballoc_lock != NULL ) liballoc_lock();		// lockit

	min = (struct liballoc_minor*)((unsigned int)ptr - sizeof( struct liballoc_minor ));

	
	if ( min->magic != LIBALLOC_MAGIC ) 
	{
		// being lied to...
		if ( liballoc_unlock != NULL ) liballoc_unlock();		// release the lock
		return;
	}

		maj = min->block;

		maj->usage -= (min->size + sizeof( struct liballoc_minor ));
		min->magic  = 0;		// No mojo.

		if ( min->next != NULL ) min->next->prev = min->prev;
		if ( min->prev != NULL ) min->prev->next = min->next;

		if ( min->prev == NULL ) maj->first = min->next;	
							// Might empty the block. This was the first
							// minor.


	// We need to clean up after the majors now....

	if ( maj->first == NULL )	// Block completely unused.
	{
		if ( l_memRoot == maj ) l_memRoot = maj->next;
		if ( maj->prev != NULL ) maj->prev->next = maj->next;
		if ( maj->next != NULL ) maj->next->prev = maj->prev;
		liballoc_free( maj, maj->pages * l_pageSize );
	}
	
	if ( liballoc_unlock != NULL ) liballoc_unlock();		// release the lock
}




void* calloc(size_t nobj, size_t size)
{
       int real_size;
       void *p;

       real_size = nobj * size;
       
       p = malloc( real_size );

       liballoc_memset( p, 0, real_size );

       return p;
}



void*   realloc(void *p, size_t size)
{
	void *ptr;
	struct liballoc_minor *min;
	int real_size;
	
	if ( size == 0 )
	{
		free( p );
		return NULL;
	}
	if ( p == NULL ) return malloc( size );

	if ( liballoc_lock != NULL ) liballoc_lock();		// lockit
		min = (struct liballoc_minor*)((unsigned int)p - sizeof( struct liballoc_minor ));
		real_size = min->size;
	if ( liballoc_unlock != NULL ) liballoc_unlock();

	if ( real_size > size ) real_size = size;

	ptr = malloc( size );
	liballoc_memcpy( ptr, p, real_size );
	free( p );

	return ptr;
}



