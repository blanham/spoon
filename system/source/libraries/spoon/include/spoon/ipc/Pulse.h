#ifndef _SPOON_IPC_PULSE_H
#define _SPOON_IPC_PULSE_H

#include <types.h>
#include <spoon/ipc/Message.h>

/** \ingroup ipc
 *
 * A Pulse is an abstraction of the system's pulse concept which 
 * is essentially a way of sending 6 unsigned ints of 32 bits
 * each to another process and/or port.
 * Pulses make use of pre-allocated memory and simple atomic
 * counters and locks. So, this is the fastest means of IPC
 * available from the system (unless you want to implement a 
 * shared memory+pulse combo system).
 *
 * A Looper is designed to listen on it's port and receive
 * system pulses, build a Pulse object and pass it through
 * to the Looper's PulseReceived() method.
 * If you want to make use of Pulses in your Application, just
 * implement your own PulseReceived method
 * 
 */

class Pulse
{
   public:
     Pulse( uint32 d1 = 0, uint32 d2 = 0, uint32 d3 = 0,
            uint32 d4 = 0, uint32 d5 = 0, uint32 d6 = 0 );
     Pulse( const Pulse& p );
	 virtual ~Pulse();
     

    int SendReply( uint32 d1, uint32 d2 = 0, uint32 d3 = 0,
			        uint32 d4 = 0, uint32 d5 = 0, uint32 d6 = 0 );
	int SendReply( Pulse *pulse );
	int SendReply( Pulse *pulse, Pulse **reply,
				  int timeout = TIMEOUT_DEFAULT );


     inline uint32 operator[] (int i) const { return _data[i]; }
     inline uint32 &operator[] (int i)       { return _data[i]; }
	


     int source_pid;  ///< The source pid of the sending application, set by the Looper.
     int source_port; ///< The source port of the sending applicaiton, set by the Looper.
     int dest_port;	  ///< The destination port of this Pulse, set by the Looper.


   private:
     uint32 _data[6]; ///< The pulse data: 6 uint32 data types.
};

#endif

