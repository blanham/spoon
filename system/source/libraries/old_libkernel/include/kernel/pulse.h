#ifndef _LIBKERNEL_PULSE_H
#define _LIBKERNEL_PULSE_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif


int32 smk_send_pulse( int32 source_port,
		  int32 dest_pid, 
		  int32 dest_port,
		  uint32 data1,
		  uint32 data2,
		  uint32 data3,
		  uint32 data4,
		  uint32 data5,
		  uint32 data6 );

int32 smk_recv_pulse( int32 *source_pid, 
		     int32 *source_port,
		     int32 *dest_port,
		     uint32 *data1, 
		     uint32 *data2, 
		     uint32 *data3,
		     uint32 *data4,
		     uint32 *data5,
		     uint32 *data6 );

int32 smk_peek_pulse( int32 *source_pid,
		     int32 *source_port,
		     int32 *dest_port,
		     uint32 *data1,
		     uint32 *data2, 
		     uint32 *data3,
		     uint32 *data4,
		     uint32 *data5,
		     uint32 *data6 );

int32 smk_waiting_pulses();

int32 smk_drop_pulse();

#ifdef __cplusplus
}
#endif
	

#endif

