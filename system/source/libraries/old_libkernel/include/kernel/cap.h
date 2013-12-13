#ifndef _LIBKERNEL_CAP_H
#define _LIBKERNEL_CAP_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

int smk_set_remote_capability( int pid, int tid, uint32 set );
int	smk_get_remote_capability( int pid, int tid, uint32 call );
int smk_set_capability( int tid, uint32 set );
int	smk_get_capability( int tid, uint32 call );

#ifdef __cplusplus
}
#endif



#endif

