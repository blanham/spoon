#include <kernel.h>


/**  \defgroup DMESG  Kernel DMESG Interfacing  
 *
 *
 */

/** @{ */

int smk_get_dmesg( void *data, int len )
{
	return _sysenter( (SYS_INFO|SYS_SIX), 1, (uint32)data,
						(uint32)len,0,0 );
}

int smk_get_dmesg_size()
{
	return _sysenter( (SYS_INFO|SYS_SIX), 0, 0,0,0,0 );
}


/** @} */


