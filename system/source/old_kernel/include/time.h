#ifndef _KERNEL_TIME_H
#define _KERNEL_TIME_H

#include <types.h>

unsigned int get_cpu_speed();

time_t epoch_uptime();
time_t epoch_seconds();
int    system_time( time_t *seconds, time_t *milliseconds );

int delay( unsigned int milliseconds );

#include <process.h>
int increment_system_time( struct process *p );

#endif

