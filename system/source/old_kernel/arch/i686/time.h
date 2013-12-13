#ifndef _KERNEL_i686_TIME_H
#define _KERNEL_i686_TIME_H

#include <types.h>
#include <process.h>

void init_time();
int increment_system_time( struct process *p );

#endif

