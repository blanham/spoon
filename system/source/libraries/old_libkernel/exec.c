#include <types.h>
#include <kernel.h>




/**  \defgroup EXEC  Process Execution  
 *
 */

/** @{ */

int32 smk_mem_exec( const char *name, 
		 uint32 start, 
		 uint32 end,
		 const char *command_line)
{
	return _sysenter( (SYS_EXEC|SYS_ONE), start, (uint32)name, end, 0, (uint32)command_line );
}

/** @} */

