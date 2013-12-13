#ifndef _KERNEL_IPC_H
#define _KERNEL_IPC_H


#include <types.h>

#define IPC_MASKED			(1<<0)


struct ipc_block
{
    int tid;
	uint32 flags;
	struct pulse *pulses;
	struct pulse *last_pulses;
	struct message *messages;
	struct message *last_message;
};


#endif

