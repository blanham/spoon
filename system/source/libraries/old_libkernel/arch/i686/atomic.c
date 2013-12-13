#include <types.h>


/**  \defgroup ATOMIC  Atomic Operations  
 *
 */


/** @{  */



void smk_acquire_spinlock ( uint32* lock )
{
  __asm__ __volatile__
        ( "spinlock_loop: mov %0, %%eax;\n"
	   "		  lock  btsl $0x0, (%%eax)\n"
	   "		  jnc got_spinlock\n"
	   " 		  int $0x30\n"
	   " 		  jmp spinlock_loop\n"
	   " got_spinlock:\n"
	  :
	  : "g" ((uint32)lock)
	  : "eax"
	  );
}

void smk_release_spinlock ( uint32* lock )
{
	__asm__ __volatile__ 
        ( " mov %0, %%eax\n"
          " release_spinlock_loop:\n"
          "	 lock\n"
  	  "	     btrl $0x0, (%%eax)\n"
	  " jnc release_spinlock_loop\n"
	  : 
	  : "g" ((uint32)lock)
	  : "eax"
	  );
}
// atomically add 1 to the number

void smk_atomic_inc( uint32* number )
{
  __asm__ __volatile__ 
        ( " mov %0, %%eax\n"
          " lock incl (%%eax)\n"
	  :
	  : "p" (number)
	  : "eax"
	  );
}


// atomically subtract 1 from the number

void smk_atomic_dec( uint32* number )
{
  __asm__ __volatile__ 
        ( " mov %0, %%eax\n"
          "  lock decl (%%eax)\n"
	  :
	  : "p" (number)
	  : "eax"
	  );
}


/** @} */

