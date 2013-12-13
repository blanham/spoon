#ifndef _KERNEL_PULSE_H
#define _KERNEL_PULSE_H

#include <types.h>
#include <threads.h>

int32 send_pulse( int source_pid, 
			     int source_port,
			     struct process *proc, 
			     int dest_port, 
			     uint32 data1,
			     uint32 data2,
			     uint32 data3,
			     uint32 data4,
			     uint32 data5,
			     uint32 data6 );

int32 receive_pulse( struct thread *tr,
		     int *source_pid,
		     int *source_port,
		     int *dest_port,
		     uint32 *data1,
		     uint32 *data2,
		     uint32 *data3,
		     uint32 *data4,
		     uint32 *data5,
		     uint32 *data6 );

int32 preview_pulse( struct thread *tr,
		     int *source_pid,
		     int *source_port,
		     int *dest_port,
		     uint32 *data1,
		     uint32 *data2,
		     uint32 *data3,
		     uint32 *data4,
		     uint32 *data5,
		     uint32 *data6 );


int32 pulses_waiting( struct thread *tr );
int32 drop_pulse( struct thread *tr );

#endif


