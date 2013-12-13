#ifndef _LIBKERNEL_MEM_H
#define _LIBKERNEL_MEM_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

 void *smk_mem_alloc( uint32 pages );
 void  smk_mem_free( void *pointer );
 int32 smk_mem_size( void *pointer );
 void *smk_mem_location( void *pointer );

 void *smk_mem_map( void *location, int pages );

			    
#ifdef __cplusplus
}
#endif

#endif

