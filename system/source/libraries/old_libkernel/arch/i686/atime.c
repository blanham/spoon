#include <types.h>


/** \addtogroup TIME
 * 
 * @{ 
 *
 */

void smk_ndelay( uint32 milliseconds )
{
  int d0;
  __asm__ __volatile__(
    "\tjmp 1f\n"
    ".align 16\n"
    "1:\tjmp 2f\n"
    ".align 16\n"
    "2:\tdecl %0\n\tjns 2b"
    :"=&a" (d0)
    :"0" (milliseconds));
}


inline void smk_release_timeslice()
{
	asm __volatile__ ( "int $0x30" );
}



/**  @}  */

