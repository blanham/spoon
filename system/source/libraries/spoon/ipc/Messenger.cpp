#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/support/String.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>


int Messenger::SendPulse( int pid, int port, Pulse *p )
{
	return Messenger::SendPulse( pid, port, 
							(*p)[0], (*p)[1],
							(*p)[2], (*p)[3],
							(*p)[4], (*p)[5] );
}
	
int Messenger::SendPulse( int pid, int port, int d1, int d2, int d3, int d4, int d5, int d6 )
{
	int local_port = smk_new_port( smk_gettid() );
	if ( local_port < 0 ) return -1;

	int ans = smk_send_pulse( local_port, pid, port, 
								d1, d2, d3,
								d4, d5, d6 );

	smk_release_port( local_port );
	return ans;
}
	


Pulse*   Messenger::SendReceivePulse( int pid, int port, Pulse *p, int timeout )
{
	return Messenger::SendReceivePulse( pid, port, 
										(*p)[0], (*p)[1],
										(*p)[2], (*p)[3],
										(*p)[4], (*p)[5],
										timeout );
}

Pulse*   Messenger::SendReceivePulse( int pid, int port, 
										int d1, int d2, int d3, int d4, int d5, int d6,
										int timeout )
{
	int local_port = smk_new_port( smk_gettid() );
	if ( local_port < 0 ) return NULL;

	struct port_pair masks[1024];
	
	smk_save_port_masks( masks );
	smk_mask_port( -1 );
	smk_unmask_port( local_port );
	
		while ( smk_drop_message() == 0 ) {}
		while ( smk_drop_pulse() == 0 ) {}
	
	
		Pulse *reply = NULL;

		int ans = smk_send_pulse( local_port, pid, port,
									d1,d2,d3,
									d4,d5,d6 );

		if ( ans == 0 )
		{
			ans = Messenger::WaitOnReply( &reply, timeout );
			if ( ans != 0 ) reply = NULL;
		}
						


	smk_restore_port_masks( masks );
	smk_release_port( local_port );
	return reply;
}





// --------------------------------------------------------


int Messenger::SendMessage( int pid, int port, Message *msg )
{
	int local_port = smk_new_port( smk_gettid() );
	if ( local_port < 0 ) return -1;
	
	int size = msg->FlattenedSize();
	void *data = malloc( size );

		msg->Flatten( (char*)data, size );

		int ans = smk_send_message( local_port, pid, port, size, data );

	smk_release_port( local_port );
	free( data );
	return ans;
}

Message* Messenger::SendReceiveMessage( int pid, int port, Message *msg, int timeout )
{
	int local_port = smk_new_port( smk_gettid() );
	if ( local_port < 0 ) return NULL;
	
	int size = msg->FlattenedSize();
	void *data = malloc( size );

		msg->Flatten( (char*)data, size );

		// Block all other ports and drop anything already waiting there.. should be nothing.

			struct port_pair masks[1024];
	
			smk_save_port_masks( masks );
			smk_mask_port( -1 );
			smk_unmask_port( local_port );
	
				while ( smk_drop_message() == 0 ) {}
				while ( smk_drop_pulse() == 0 ) {}
		
				int ans = smk_send_message( local_port, pid, port, size, data );

				Message *reply = NULL;

				if ( ans == 0 )
				{
					ans = Messenger::WaitOnReply( &reply, timeout );
					if ( ans != 0 ) reply = NULL;
				}

			smk_restore_port_masks( masks );

	smk_release_port( local_port );
	free( data );
	return reply;
}


// ------------------------------------------------------------


int Messenger::WaitOnReply( Pulse **pulse, int timeout )
{

  int source_pid = -1;
  int source_port = -1;
  int dest_port = -1;
  uint32 d1,d2,d3,d4,d5,d6;
 
  if ( smk_go_dormant_t( timeout ) != 0 ) return -1;

  // Check for pulses		
     if ( smk_recv_pulse( &source_pid, &source_port, &dest_port,
                         &d1, &d2, &d3, &d4, &d5, &d6) == 0 )
     {

         *pulse = new Pulse( d1,d2,d3,d4,d5,d6 );

	       (*pulse)->source_pid  = source_pid;
	       (*pulse)->source_port = source_port;
	       (*pulse)->dest_port   = dest_port;

		 return 0;
      }

  return -1;
}

int Messenger::WaitOnReply( Message **msg, int timeout )
{
   int source_pid;
   int source_port;
   int dest_port;
   int length;

   
   if ( smk_go_dormant_t( timeout ) !=  0 ) return -1; 
      
   if ( smk_peek_message( &source_pid, &source_port, &dest_port, &length ) == 0 )
   {
     void *data = malloc( length );

     if ( smk_recv_message( &source_pid, &source_port, &dest_port, length, data ) >= 0 )
     {
        *msg = new Message();
		if ( (*msg)->Unflatten( (const char*)data ) != -1 )
		{
		  (*msg)->_source_pid   = source_pid;
		  (*msg)->_source_port  = source_port;
		  (*msg)->_dest_port  = dest_port;
	      free( data );
		  return 0;
	    }
     }

     free( data );
   }

 return -1;
}



// NAMED OPERATIONS ------

int Messenger::SendPulse( const char *process, int port, Pulse *p )
{
	return Messenger::SendPulse( smk_find_process_id( process ), port, p );
}

int Messenger::SendPulse( const char *process, int port, int d1, int d2, int d3, int d4, int d5, int d6 )
{
	return Messenger::SendPulse( smk_find_process_id( process ), port, d1,d2,d3,d4,d5,d6 );
}

Pulse*   Messenger::SendReceivePulse( const char *process, int port, Pulse *p, int timeout )
{
	return Messenger::SendReceivePulse( smk_find_process_id( process ), port, p, timeout );
}

Pulse*   Messenger::SendReceivePulse( const char *process, int port,
											int d1, int d2, int d3, 
											int d4, int d5, int d6,
											int timeout )
{
	return Messenger::SendReceivePulse( smk_find_process_id( process ), port, d1,d2,d3,d4,d5,d6, timeout );
}

	
			// ---------------- messages ---------------------------------------

int Messenger::SendMessage( const char *process, int port, Message *msg )
{
	return Messenger::SendMessage( smk_find_process_id( process ), port, msg );
}

Message* Messenger::SendReceiveMessage( const char *process, int port, Message *msg, int timeout )
{
	return Messenger::SendReceiveMessage( smk_find_process_id( process ), port, msg, timeout );
}





