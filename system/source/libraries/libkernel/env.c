#include <kernel.h>




/**  \defgroup ENV  Environment Variables  
 *
 */

/** @{ */

int smk_setenv( const char *name, void *data, int size )
{
	return _sysenter( (SYS_PROCESS|SYS_FIVE), 0, (unsigned int)name, 
												 (unsigned int)data,size,0 );
}

int smk_getenv( const char *name, void *data, int size )
{
	return _sysenter( (SYS_PROCESS|SYS_FIVE), 1, (unsigned int)name, 
												 (unsigned int)data, size,0 );
}

int smk_getenv_size( const char *name )
{
	int size;

	if ( _sysenter( (SYS_PROCESS|SYS_FIVE), 2,  
							(unsigned int)name, 
							(unsigned int)&size,0,0 ) == 0 )
			return size;

	return -1;
}

int smk_getenv_info( int i, const char *name, int *size )
{
	return _sysenter( (SYS_PROCESS|SYS_FIVE), 3, i, 
											(unsigned int)name, (unsigned int)size,0 ); 
}

int smk_clearenv( const char *name )
{
	return _sysenter( (SYS_PROCESS|SYS_FIVE), 4, (unsigned int)name, 0,0,0 ); 
}


/** @} */


