#ifndef _SPOON_IPC_LOOPER_H
#define _SPOON_IPC_LOOPER_H

#include <types.h>
#include <spoon/base/Thread.h>



class Message;
class Pulse;

/** \ingroup ipc 
 *
 * A Looper class is an abstraction of the system's IPC system and it's
 * job is to listen and receive Messages and Pulses which are incoming
 * on different IPC ports.
 *
 * The Looper class is a Thread and, so, you need to Resume() any Loopers
 * or classes which inherited Loopers once you have instantiated them. Also
 * note that Loopers are auto-deleting Thread's, so you should never delete
 * them. Instead, call Quit() and  override the QuitRequested() method to do 
 * your cleanups.
 */


class Looper : public Thread
{

   public:
     Looper( int port = 0 );
     Looper( const char *name, int port = 0 );
     virtual ~Looper();


	int PostMessage( Message *msg );
	int PostPulse( Pulse *pulse );
	int PostPulse( uint32 d1, uint32 d2 = 0, uint32 d3 = 0, uint32 d4 = 0, uint32 d5 = 0, uint32 d6 = 0 );
	 
    int Port();
 

	virtual void MessageReceived( Message *msg );
	virtual void PulseReceived( Pulse *pulse );
    virtual void QuitRequested();

	int Quit();

    virtual int Run();
	
   private:
     int  m_port;			/**< IPC port that the Looper is listening on. */

};

#endif


