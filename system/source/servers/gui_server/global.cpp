#include <stdlib.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Message.h>
#include "global.h"


int unknown_message( Message *msg )
{
	msg->SendReply( MESSAGE_NOT_UNDERSTOOD ); 
	return 0;
}


int msg_error( Message *msg, char *error, bool del )
{
	Message *reply = new Message( OK );

	 reply->AddInt( "rc", -1 );
	 
	 if ( error != NULL )
		 reply->AddString( "_error", error );
	
		msg->SendReply( reply ); 

	delete reply;

	if ( del == true ) free( error );
	 
	return 0;
}


