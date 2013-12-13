#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/ipc/Pulse.h>

#include <spoon/interface/Rect.h>
#include <spoon/interface/InterfaceDefs.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/ipc/Message.h>
#include <spoon/collections/List.h>

#include <GuiServer.h>
#include <global.h>




#define BGCOLOR			0x000050


// ******************* CONSTRUCTORS *******************


GuiServer::GuiServer()
: Application("gui_server")
{
	desktop = new GUI::Desktop();
	desktop->Resume();
}

GuiServer::~GuiServer()
{
	desktop->Quit();
	desktop->Wait();
	if ( desktop != NULL ) delete desktop;
}


// ***************************************************

// **************************************************

int GuiServer::registration_message( Message *msg )
{
	char *type = NULL;

	if ( msg->FindString( "_type", (const char**)&type ) != 0 )
			return msg_error( msg, "no _type found" );


	  if ( strcmp( type, "window" ) == 0 ) 			
	  {
		register_window( msg );
	  }
//	  else if ( strcmp( type, "direct_window" ) == 0 )	
//	  {
//	  	register_direct_window( msg );
//	  }
//	  else if ( strcmp( type, "video_driver" ) == 0 )
//	  {
//		register_video_driver( msg );
//	  }
	  else
	  {
		msg_error( msg, "unknown _type. Try: window, direct_window, video_driver.");
	  }
	

	free( type );
		
	return 0;
}


int GuiServer::get_message( Message *msg )
{
	char *type = NULL;

	if ( msg->FindString( "_type", (const char**)&type ) != 0 )
			return msg_error( msg, "no _type found" );


	  if ( strcmp( type, "desktop_mode" ) == 0 ) 			
	  {
		Message *reply = new Message(0);
				 reply->AddInt( "rc",  0 );
				 reply->AddInt( "_id", 0 );
				 reply->AddInt( "_width", desktop->Bounds().Width() );
				 reply->AddInt( "_height", desktop->Bounds().Height() );
				 reply->AddInt( "_mode", 0 );

		msg->SendReply( reply );
	  }
//	  else if ( strcmp( type, "direct_window" ) == 0 )	
//	  {
//	  	register_direct_window( msg );
//	  }
//	  else if ( strcmp( type, "video_driver" ) == 0 )
//	  {
//		register_video_driver( msg );
//	  }
	  else
	  {
		msg_error( msg, "unknown _type. Try: desktop_mode.");
	  }
	

	free( type );

		
	return 0;
}


// ***************************************************




// ***************************************************

void GuiServer::MessageReceived( Message *msg )
{

  switch ( msg->what )
  {
     case REGISTER:
		registration_message( msg );
		break;


	case GET:
		get_message( msg );
		break;

	 
	
     default:
	  	Application::MessageReceived( msg );
		break;
  }

}



void GuiServer::PulseReceived( Pulse *p )
{
   //int pid  = p->source_pid;
   uint32 d1,d2,d3,d4,d5,d6;
   d1 = (*p)[0];
   d2 = (*p)[1];
   d3 = (*p)[2];
   d4 = (*p)[3];
   d5 = (*p)[4];
   d6 = (*p)[5];
   


   switch (d1)
   {
     case MOUSE_MOVED:
     case MOUSE_DOWN:
     case MOUSE_UP:
           desktop->MouseEvent( d1, d2, d3, d4 );
	   	   break;

     case KEY_UP:
     case KEY_DOWN:
		   desktop->KeyPressed( d1, d2, d3, d4 );
           break;

	 

     default:
       Application::PulseReceived( p );
       break;
   }
   
}






