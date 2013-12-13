#ifndef _LIBKERNEL_ATOMIC_H
#define _LIBKERNEL_ATOMIC_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void smk_acquire_spinlock ( uint32* );
void smk_release_spinlock ( uint32* );
void smk_atomic_dec( uint32* number );
void smk_atomic_inc( uint32* number );

#ifdef __cplusplus
}
#endif

#endif

