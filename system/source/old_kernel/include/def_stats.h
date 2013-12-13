#ifndef _KERNEL_DEF_STATS_H
#define _KERNEL_DEF_STATS_H


struct stats
{
	int memory_user;
	int memory_system;

	int seconds_user;
	int seconds_system;
	int milliseconds_user;
	int milliseconds_system;
};

#endif

