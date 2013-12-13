#include <kernel.h>



/**  \defgroup MISC  Miscellaneous Stuff  
 *
 */

/** @{ */

void  smk_disable_interrupts() { __asm__ (" cli "); }
void  smk_enable_interrupts() { __asm__ (" sti "); }


void smk_reboot()
{
	_sysenter( SYS_REBOOT | SYS_ONE ,0,0,0,0,0 );
}



int smk_go_dormant()
{
   return _sysenter( (SYS_THREAD|SYS_FOUR) , 0,0,0,0,0 );
}


int smk_go_dormant_t( int milliseconds )
{
   return _sysenter( (SYS_THREAD|SYS_FOUR) , milliseconds,0,0,0,0 );
}


void smk_exit(int code)
{
	_sysenter( (SYS_PROCESS|SYS_THREE), code, 0, 0,0,0 );
}


/** @} */

