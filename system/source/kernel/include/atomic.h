#ifndef _KERNEL_ATOMIC_H
#define _KERNEL_ATOMIC_H


extern volatile unsigned int kernel_lock;


static inline void acquire_spinlock( volatile unsigned int *lock )
{
  int gotit = 0;

  while ( 1==1 )
  {
	asm volatile ( "lock btsl $0x0, (%1)\n"
              	   "setnc %0\n "
			  		: "=g" (gotit)
			  		: "p" (lock)
				);

	if ( gotit != 0 ) break;
	asm ( "pause" );	// As per the Intel recommendations.
	/// \todo sched_yield();
  }
}


static inline void release_spinlock( volatile unsigned int *lock )
{
	asm volatile ( "lock btrl $0x0, (%0)\n" : : "p" (lock) );
}


static inline void atomic_inc( int *number )
{
  asm volatile ( "lock incl (%0)\n" : : "p" (number) );
}


static inline void atomic_dec( int *number )
{
  asm volatile ( "lock decl (%0)\n" : : "p" (number) );
}




static inline void begin_critical_section()
{
	acquire_spinlock( & kernel_lock );
}

static inline void end_critical_section()
{
	release_spinlock( & kernel_lock );
}



#endif

