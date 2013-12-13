#ifndef _KERNEL_DEF_PULSE_H
#define _KERNEL_DEF_PULSE_H

#include <types.h>

struct pulse
{
	int source_pid;
	int source_port;
	int dest_port;
	uint32 data1;
	uint32 data2;
	uint32 data3;
	uint32 data4;
	uint32 data5;
	uint32 data6;
} __attribute__ ((packed));


#endif

