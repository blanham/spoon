#include <types.h>
#include <atomic.h>
#include <switching.h>
#include <conio.h>
#include <interrupts.h>
#include <scheduler.h>
#include <time.h>
#include <params.h>


void acquire_spinlock( spinlock *lock )
{
  int gotit = 0;

  while ( 1==1 )
  {
	__asm__ __volatile__
			( "    lock btsl $0x0, (%1)\n"
              "    jc 1f\n "
			  "    movl $0x1, %0\n "
              " 1:\n "
			  : "=g" (gotit)
			  : "p" (&(lock->lock)) 
			);

	if ( gotit == 1 ) break;
	sched_yield();
  }

  lock->thread = current_thread();
}

void release_spinlock( spinlock *lock )
{
	__asm__ __volatile__ 
	( "lock btrl $0x0, (%0)\n" : : "p" (&(lock->lock)) );
}

int check_spinlock( spinlock *lock )
{
	return lock->lock;
}


int atomic_inc( int *number )
{
  __asm__ __volatile__ ( "lock incl (%0)\n" : : "p" (number) );
  return 0;
}


int atomic_dec( int *number )
{
  __asm__ __volatile__ ( "lock decl (%0)\n" : : "p" (number) );
  return 0;
}


