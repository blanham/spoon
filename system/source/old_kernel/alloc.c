#include <types.h> 
#include <dmesg.h>
#include <atomic.h>
#include <memory.h>

/** Supporting functions for the liballoc library file */

static spinlock lock = INIT_SPINLOCK;


int liballoc_lock()
{
	acquire_spinlock( &lock );
	return 0;
}

int liballoc_unlock()
{
	release_spinlock( &lock );
	return 0;
}


void* liballoc_alloc( int pages )
{
	return memory_alloc( pages );
}

int liballoc_free( void* ptr, int pages )
{
	memory_free( pages, ptr );
	return 0;
}



