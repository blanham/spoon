#ifndef _LIBKERNEL_PROCESS_H
#define _LIBKERNEL_PROCESS_H

#include <types.h>
#include <kernel/lengths.h>

#ifdef __cplusplus
extern "C" {
#endif

int32 smk_find_process_id( const char name[OS_NAME_LENGTH] );
int32 smk_find_process_name( int32 pid, const char *name );

int32 smk_process_exists( int32 pid );

int32 smk_getpid();
int32 smk_start_process( int32 pid );


#ifdef __cplusplus
}
#endif



#endif

