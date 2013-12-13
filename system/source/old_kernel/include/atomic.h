#ifndef _KERNEL_ATOMIC_H
#define _KERNEL_ATOMIC_H

#include <types.h>

void acquire_spinlock( spinlock *lock );
void release_spinlock( spinlock *lock );

int check_spinlock( spinlock *lock );

int atomic_inc( int *number );
int atomic_dec( int *number );

#endif


