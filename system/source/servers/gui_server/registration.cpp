#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <spoon/sysmsg.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Window.h>
#include <spoon/ipc/Message.h>

#include <global.h>

#include <GuiServer.h>


#include <Window.h>


// ************************************************************

int GuiServer::register_window( Message *msg )
{
	int pid;
	int port;
	int did;
	char *title;
	bool visible;
	unsigned int flags;
	Rect frame;


	// Gather the information
	
	pid  = msg->source_pid();

	if ( msg->FindInt( "_flags", (int*)&flags ) != 0 ) 
			return msg_error( msg, "no _flags found" );
	
	if ( msg->FindInt( "_port", &port ) != 0 ) 
			return msg_error( msg, "no _port found" );
	
	if ( msg->FindRect( "_rect", &frame ) != 0 )
			return msg_error( msg, "no _rect found" );

	if ( frame.Width() <= 0 ) 
			return msg_error( msg, "invalid frame rect" );
	
	if ( frame.Height() <= 0 ) 
			return msg_error( msg, "invalid frame rect" );
	
	if ( msg->FindString( "_title", (const char**)&title ) != 0 )
			return msg_error( msg, "no _title found");
	
	if ( msg->FindBool( "_visible", &visible ) != 0 ) visible = true;

	if ( msg->FindInt( "_did", &did ) != 0 ) did = -1;


	// Do the actual request 

	
	GUI::Window *win = GUI::Window::create( frame, title, pid, port, visible, flags ); 
	free( title );

	printf("%s%x\n","WINDOW: ", win );
	
	// Return error if failed.
	
	if ( win == NULL ) 
			return msg_error( msg, "unknown error" );

	printf("%s%x\n","Desktop: ", desktop );

	if ( (win->flags() & WINDOW_DESKTOP) != WINDOW_DESKTOP )
	{
			printf("%s%x\n","ADDING WINDOW: ", win );
				desktop->add( win ); // Add to the desktop.
	}
		else	
		{
			printf("%s%x\n","ADDING WINDOW: ", win );
			desktop->setDesktopWindow( win );
		}

	// Assume that the person requesting this is waiting..
	Message *reply = new Message( OK );

			reply->AddInt( "rc", 0 );
			reply->AddInt( "_id", win->wid() );
			reply->AddInt( "_sid", win->sid() );
			reply->AddInt( "_did", desktop->Port() );
			reply->AddRect( "_rect", win->Frame() );

			// Send success reply with information.
			if ( msg->SendReply( reply ) != 0 )
			{
				// \todo freak out.
			}

	delete reply;

	return 0;
}



// ************ MAIN REGISTRATION SWITCH **********************



