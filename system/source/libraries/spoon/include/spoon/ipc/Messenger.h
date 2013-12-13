#ifndef _SPOON_IPC_MESSENGER_H
#define _SPOON_IPC_MESSENGER_H

#include <types.h>
#include <kernel.h>

#include <stdlib.h>

#include <spoon/support/String.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>



/** \ingroup ipc 
 *
 *
 */
class Messenger 
{
    public:

			
	// -------- NAMED OPERATIONS -----------------------------------
			
		static int SendPulse( const char *process, int port, Pulse *p );
		static int SendPulse( const char *process, int port, int d1, int d2 = 0, int d3 = 0, int d4 = 0, int d5 = 0, int d6 = 0 );
		static Pulse*   SendReceivePulse( const char *process, int port, Pulse *p, int timeout = TIMEOUT_DEFAULT );
		static Pulse*   SendReceivePulse( const char *process, int port, 
											int d1, int d2 = 0, int d3 = 0, 
											int d4 = 0, int d5 = 0, int d6 = 0,
											int timeout = TIMEOUT_DEFAULT );
	
			// ---------------- messages ---------------------------------------

		static int SendMessage( const char *process, int port, Message *msg );
		static Message* SendReceiveMessage( const char *process, int port, Message *msg, int timeout = TIMEOUT_DEFAULT );


			

			
   	// ----------------------- pulses ------------------------------

		static int SendPulse( int pid, int port, Pulse *p );
		static int SendPulse( int pid, int port, int d1, int d2 = 0, int d3 = 0, int d4 = 0, int d5 = 0, int d6 = 0 );
		static Pulse*   SendReceivePulse( int pid, int port, Pulse *p, int timeout = TIMEOUT_DEFAULT );
		static Pulse*   SendReceivePulse( int pid, int port, 
											int d1, int d2 = 0, int d3 = 0, 
											int d4 = 0, int d5 = 0, int d6 = 0,
											int timeout = TIMEOUT_DEFAULT );
	
	// ---------------- messages ---------------------------------------

		static int SendMessage( int pid, int port, Message *msg );
		static Message* SendReceiveMessage( int pid, int port, Message *msg, int timeout = TIMEOUT_DEFAULT );


	// ---------------- static waiting -----------
	
	
	
	static int WaitOnReply( Message **reply, int timeout = TIMEOUT_DEFAULT );
	static int WaitOnReply( Pulse **reply, int timeout = TIMEOUT_DEFAULT );
       
		
};


#endif


