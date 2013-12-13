#include <types.h>
#include <kernel.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Messenger.h>

/** The Pulse constructor, accepting 6 uint32 types as initialization
 * parameters. The default values for each uint32 is 0 so you can
 * just create a Pulse by only adding the values you use.
 */
Pulse::Pulse( uint32 d1, uint32 d2, uint32 d3,
              uint32 d4, uint32 d5, uint32 d6 )
{
  source_port = -1;
  source_pid  = -1;
  dest_port   = -1;
  _data[0] = d1;
  _data[1] = d2;
  _data[2] = d3;
  _data[3] = d4;
  _data[4] = d5;
  _data[5] = d6;
}

/** Copy constructor. Because the data types are just uint32's, the entire 
 * Pulse is copied an you are free to delete the original. They will be
 * exactly the same.
 */
Pulse::Pulse( const Pulse& p )
{
  source_pid  = p.source_pid;
  source_port = p.source_port;
  dest_port   = p.dest_port;
  
  for ( int i = 0; i < 6; i++ ) _data[i] = p._data[i];
}

Pulse::~Pulse()
{
}
     
// ----------------------------------------

/** Sends a reply to the originating source_pid:source_port. Of course,
 * the Pulse needs to have been received from a remote source by the
 * Looper so that source_pid and source_port point to valid return address.
 * \return 0 on successful reply.
 */
int Pulse::SendReply( uint32 d1, uint32 d2, uint32 d3,
		       uint32 d4, uint32 d5, uint32 d6 )
{
   return smk_send_pulse( dest_port, source_pid, source_port,
   			d1, d2, d3, d4, d5, d6 );
}

/** Sends a reply to the originating source_pid:source_port. Of course,
 * the Pulse needs to have been received from a remote source by the
 * Looper so that source_pid and source_port point to valid return address.
 * \return 0 on successful reply.
 */
int Pulse::SendReply( Pulse *p )
{
   return smk_send_pulse( dest_port, source_pid, source_port,
   		      (*p)[0], (*p)[1], (*p)[2], (*p)[3], (*p)[4], 
   		      (*p)[5] );
}

/** Sends a reply to the originating source_pid:source_port and then
 * waits for a reply. The reply Pulse is created at Pulse** and
 * it's up to you to delete it.
 *
 * \param pulse The response pulse which is sent.
 * \param reply A pointer to a pointer to a Pulse which will contain the 
 * 			reply.
 * \param timeout The timeout in milliseconds to wait for a reply.
 * \return 0 on successful reply and reply either contains the reply
 * or NULL.
 */
int Pulse::SendReply( Pulse *pulse, Pulse** reply,
			int timeout)
{
   *reply = Messenger::SendReceivePulse( source_pid, source_port, pulse, timeout );
   return 0;
}



