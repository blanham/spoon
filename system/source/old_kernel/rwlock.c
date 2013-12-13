#include <types.h>
#include <interrupts.h>
#include <alloc.h>
#include <rwlock.h>
#include <conio.h>
#include <scheduler.h>
#include <switching.h>
#include <params.h>



/*
 * A reader-writer locking implementation.
 *
 */

 /// \todo Add a mutex queue instead of the sched-yielding.

struct rwlock *init_rwlock()
{
	struct rwlock *rw = (struct rwlock*)malloc(sizeof(struct rwlock));

		rw->num_readers   = 0;
		rw->num_writers   = 0;
		rw->write_request = 0;

	return rw;
}


int delete_rwlock( struct rwlock *rw )
{
	free( rw );
	return 0;
}

int rwlock_get_write_access( struct rwlock *rw )
{
	while ( 1==1 )
	{
		int enabled = disable_interrupts();

			if ( (rw->num_readers == 0) && (rw->num_writers == 0) )
			{
				rw->num_writers += 1;
				if ( enabled == 1 ) enable_interrupts();
				break;
			}
			else rw->write_request = 1;
	
		if ( enabled == 1 ) enable_interrupts();

		sched_yield();
	}

	return 0;
}


int rwlock_get_read_access( struct rwlock *rw )
{
	
	while ( 1==1 )
	{
		int enabled = disable_interrupts();

			if (  (rw->num_writers == 0) && (rw->write_request == 0) )
			{
				rw->num_readers += 1;
				if ( enabled == 1 ) enable_interrupts();
				break;
			}
	
		if ( enabled == 1 ) enable_interrupts();
		
		sched_yield();
	}
	
	return 0;

}

int rwlock_release( struct rwlock *rw )
{
	int enabled = disable_interrupts();

		if ( rw->num_writers != 0 )
		{
			rw->write_request = 0;
			rw->num_writers   = 0;
		}
		else
		{
			rw->num_readers -= 1;
		}
	
	if ( enabled == 1 ) enable_interrupts();
	return 0;
}






