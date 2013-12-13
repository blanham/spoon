#include <kernel.h>




/**  \defgroup RANDOM  Kernel-level Random Number Generator  
 *
 *
 */

/** @{ */

int smk_random()
{
    return _sysenter( (SYS_INFO|SYS_FOUR), 0, 0, 0, 0, 0 );
}

/** @} */


