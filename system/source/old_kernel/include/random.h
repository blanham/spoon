#ifndef _KERNEL_RANDOM_H
#define _KERNEL_RANDOM_H

#include <types.h>

#define RANDOM_POOL_SIZE	16

extern int random_pool[ RANDOM_POOL_SIZE ];

int init_random();
int add_random( int value1, int value2 );
int random();

#endif

