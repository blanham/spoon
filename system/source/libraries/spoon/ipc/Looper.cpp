#include <types.h>
#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <spoon/sysmsg.h>
#include <spoon/base/Thread.h>
#include <spoon/ipc/Looper.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>



/** When constructing a Looper using this constructor, you can specify
 * which IPC port the Looper should listen to. If it is unable to 
 * acquire the specified port, then the first available port is used
 * instead.
 *
 * During construction, the auto-delete feature of the derived Thread
 * class is set to true.
 *
 * The default name for a Looper Thread is "_looper".
 */
Looper::Looper( int port )
: Thread( "_looper" ), m_port(port)
{
   SetAutoDelete( true );
  
   int ok = smk_create_port( m_port, tid() );

   if ( ok != 0 ) 
   {
   	ok = smk_new_port( tid() );
	m_port = ok;
   }

}

/** Use this constructor if you want to give your Looper a specific
 * name. Otherwise, all behaviour is exactly similar to the 
 * Looper( int port ) constructor.
 */

Looper::Looper( const char *name, int port )
: Thread( name ), m_port(port)
{
   SetAutoDelete( true );

   int ok = smk_create_port( m_port, tid() );

   if ( ok != 0 ) 
   {
   	ok = smk_new_port( tid() );
	m_port = ok;
   }
   
}


/** The destructor should never be called from within your
 * program. Instead, use Quit() to signal that the Looper
 * must shutdown and override QuitRequested() to do your
 * cleanups.
 * 
 * The Looper's Thread will automatically delete the Looper
 * when the main IPC loop returns.
 */
Looper::~Looper()
{
   smk_release_port( m_port );
}



/** Any received messages on the Looper's IPC port are
 * passed to this virtual method. You will almost definitely
 * be overriding this method in your classes when you build
 * up your own Windows or Applications.
 *
 * \warning Do not delete the Message parameter that you are given. It 
 * 			does not belong to you.
 *
 * \param msg A pointer to the received Message. 
 */
void Looper::MessageReceived( Message* msg )
{
}


/** Any pulses received on the Looper's IPC port are
 * passed to this virtual method. 
 *
 * \warning Do not delete the Pulse parameter that you are given. It does 
 * 		    not belong to you either.
 *
 * \param pulse A pointer to the received Pulse. 
 */
void Looper::PulseReceived( Pulse* pulse )
{
	switch ((*pulse)[0])
	{
		case QUIT_REQUESTED:
			QuitRequested();
			Terminate();
			break;
	}
}



// --------------------------------------------------------

/** Posts into the Looper's queue. Use this instead of MessageRecieved or PulseReceived.
 * \return 0 if the post was successful.
 */
int Looper::PostMessage( Message *msg )
{
	return Messenger::SendMessage( tid(), m_port, msg ); 
}

/** Posts into the Looper's queue. Use this instead of MessageRecieved or PulseReceived.
 * \return 0 if the post was successful.
 */
int Looper::PostPulse( Pulse *pulse )
{
	return Messenger::SendPulse( tid(), m_port, pulse );
}

/** Posts into the Looper's queue. Use this instead of MessageRecieved or PulseReceived.
 * \return 0 if the post was successful.
 */
int Looper::PostPulse( uint32 d1, uint32 d2, uint32 d3, uint32 d4, uint32 d5, uint32 d6 )
{
	return Messenger::SendPulse( tid(), m_port, d1,d2,d3,d4,d5,d6 );
}



/** Call this method when you want to the Looper to finish. Internally, 
 * Quit() sends a Pulse to it's own Looper with the QUIT_REQUESTED
 * value. This, in turn, invokes the QuitRequested() method before 
 * the Looper returns.
 *
 * \return 0 if the quit request want sent successfully.
 */
int Looper::Quit()
{
  return PostPulse( QUIT_REQUESTED ); 
}

/** This is a virtual method which any inherited classes can override
 * to do their own cleanups just before the Looper quits. It is
 * called from within the context of the Looper thread.
 */
void Looper::QuitRequested() 
{ 
}


/** This returns the IPC port number that the Looper is listening
 * on. Of course, any value less than 0 is not good. The main
 * Application should usually acquire port 0 first so you shouldn't
 * attempt to get that one unless you know what you are doing.
 *
 * \return the Looper's IPC port number.
 */
int Looper::Port()
{ 
	return m_port; 
} 



/** The Looper's Run() method - which is executed within the context
 * of the Looper's thread - is responsible for listening on the Looper's
 * port. It receives any Message and Pulse which is sent to it.
 *
 * The Looper waits on incoming IPC and handles it on a case-by-case
 * basis. The loop itself is checking Thread's Terminated() flag on
 * each iteration.  Hence, calling Terminate() on the Looper() will not
 * do much until the Looper is woken up by an IPC and it has a chance
 * to check the Terminated() flag.
 *
 * However, you don't want to do this.  Call Quit() instead. It will
 * wrap things up nicely.
 * 
 * \return 0 for a successful finish.
 */
int Looper::Run()
{
   int source_pid;
   int source_port;
   int dest_port;
   uint32 d1,d2,d3,d4,d5,d6;
   int length;


   while ( Terminated() == false  )
   {
      smk_go_dormant();

		// Check for pulses		
      if ( smk_recv_pulse( &source_pid, 
  		    			  &source_port, 
						  &dest_port,
                          &d1, &d2, &d3, &d4, &d5, &d6) == 0 )
      {
      
         Pulse p( d1,d2,d3,d4,d5,d6 );
	       p.source_pid  = source_pid;
	       p.source_port = source_port;
	       p.dest_port   = dest_port;

		   PulseReceived( &p );
      }

      if ( smk_peek_message( &source_pid, &source_port, &dest_port, &length ) == 0 )
      {
	     void *data = malloc( length );

	     if ( smk_recv_message( &source_pid, &source_port, &dest_port, length, data ) >= 0 )
	     {
	        Message *msg = new Message();
			if ( msg->Unflatten( (const char*)data ) != -1 )
			{
			  msg->_source_pid   = source_pid;
			  msg->_source_port  = source_port;
			  msg->_dest_port    = dest_port;
		
			  MessageReceived( msg );
	        }
			delete msg;
	     }

	     free( data );
      }
	   
   }

     return 0;
}


