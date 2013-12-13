#include <kernel.h>


/**  \defgroup PULSE  Pulse IPC  
 

<P>
Pulses are the simplest and fastest means of IPC available. It allows one process to send
6 values (unsigned 32 bit integers) to another process. The pulse is asynchronous and 
non-blocking, meaning that if you send it, it's gone and the sending process will continue
immediately.  Maybe the recipient is going to use it, maybe it isn't.



  
 */

/** @{ */

int32 smk_send_pulse( int32 source_port,
		  int32 dest_pid, 
		  int32 dest_port,
		  uint32 data1,
		  uint32 data2,
		  uint32 data3,
		  uint32 data4,
		  uint32 data5,
		  uint32 data6 )
{
	uint32 data[6];

	data[0] = data1;
	data[1] = data2;
	data[2] = data3;
	data[3] = data4;
	data[4] = data5;
	data[5] = data6;

	return _sysenter( (SYS_PULSE|SYS_ONE), source_port, dest_pid, dest_port, (uint32)&data, 0 );
	
}



int32 smk_recv_pulse( int32 *source_pid, 
		     int32 *source_port,
		     int32 *dest_port,
		     uint32 *data1, 
		     uint32 *data2, 
		     uint32 *data3,
		     uint32 *data4,
		     uint32 *data5,
		     uint32 *data6 )
{
	uint32 data[6];
	int32 ans;

	ans = _sysenter( (SYS_PULSE|SYS_TWO), 
			  (uint32)source_pid, 
			  (uint32)source_port, 
			  (uint32)dest_port,
			  (uint32)&data, 0 );

	*data1 = data[0];
	*data2 = data[1];
	*data3 = data[2];
	*data4 = data[3];
	*data5 = data[4];
	*data6 = data[5];

	return ans;
}

int32 smk_peek_pulse( int32 *source_pid, 
		     int32 *source_port,
		     int32 *dest_port,
		     uint32 *data1, 
		     uint32 *data2, 
		     uint32 *data3,
		     uint32 *data4,
		     uint32 *data5,
		     uint32 *data6 )
{
	uint32 data[6];
	int32 ans;

	ans = _sysenter( (SYS_PULSE|SYS_TWO), 
			  (uint32)source_pid, 
			  (uint32)source_port, 
			  (uint32)dest_port,
			  (uint32)&data, 0 );

	*data1 = data[0];
	*data2 = data[1];
	*data3 = data[2];
	*data4 = data[3];
	*data5 = data[4];
	*data6 = data[5];

	return ans;
}


int32 smk_waiting_pulses()
{
	return _sysenter( (SYS_PULSE|SYS_FOUR), 0,0,0,0,0 );
}

int32 smk_drop_pulse()
{
	return _sysenter( (SYS_PULSE|SYS_FIVE), 0,0,0,0,0 );
}



/** @} */

