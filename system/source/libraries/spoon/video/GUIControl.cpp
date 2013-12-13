#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/collections/List.h>
#include <spoon/video/GUIControl.h>

GUIControl::GUIControl()
{
}

GUIControl::~GUIControl()
{
}


int GUIControl::CreateDesktop( char *name, char *description, int *id )
{
		Message *msg = new Message( CREATE );
				 msg->AddString( "_type", "desktop" );
				 msg->AddString( "_name", name );
				 msg->AddString( "_description", description );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0 , msg );
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;

		if ( reply->rc() != 0 ) 
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
			delete reply;
			return -1;
		}


		if ( reply->FindInt( "_id", id ) != 0 ) 
		{
			delete reply;
			return -1;
		}

		delete reply;
		
	return 0;
}

int GUIControl::DeleteDesktop( int desktop_id )
{
		Message *msg = new Message( DELETE );
				 msg->AddString( "_type", "desktop" );
				 msg->AddInt( "_id", desktop_id );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;

		if ( reply->rc() != 0 ) 
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
			delete reply;
			return -1;
		}


		delete reply;
		
	return 0;
}
		


int GUIControl::GetDriverList( List *drivers )
{
		Message *msg = new Message( GET );
				 msg->AddString( "_type", "driver_list" );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
		
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;

		if ( reply->rc() != 0 ) 
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
			delete reply;
			return -1;
		}
		
		// --------------------------------
		
		int id;
		char *name;
		char *description;
		
		int count = 0;
		while ( (reply->FindInt( "_id", count, &id )) == 0 )
		{
		   if ( reply->FindString( "_description", count, (const char**)&description ) == 0 )
			if ( reply->FindString( "_name", count, (const char**)&name ) == 0 )
			 {
				struct video_driver_info *vd = new struct video_driver_info;

					vd->id = id;
					strncpy( vd->name, name, 1024 );
					strncpy( vd->description, description, 1024 );

				drivers->add( vd );

				free( name );
				free( description );
			 }
			else free( description );

			count += 1;
		}
	

		// --------------------------------
				
	delete reply;
	return 0;
}

int GUIControl::GetDesktopList( List *desktops )
{
		Message *msg = new Message( GET );
				 msg->AddString( "_type", "desktop_list" );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
				 
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;

		if ( reply->rc() != 0 ) 
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
			delete reply;
			return -1;
		}
		
		// --------------------------------
		
		int id;
		int vid;
		char *name;
		char *description;
		
		int count = 0;
		while ( (reply->FindInt( "_id", count, &id )) == 0 )
		{
		  if ( reply->FindInt( "_vid", count, &vid ) == 0 )
		   if ( reply->FindString( "_description", count, (const char**)&description ) == 0 )
			if ( reply->FindString( "_name", count, (const char**)&name ) == 0 )
			 {
				struct desktop_info *desk = new struct desktop_info;

					desk->id = id;
					desk->video_driver = vid;
					strncpy( desk->name, name, 1024 );
					strncpy( desk->description, description, 1024 );

				desktops->add( desk );

				free( name );
				free( description );
			 }
			else free( description );

			count += 1;
		}
	

		// --------------------------------
				
	delete reply;
	
	return 0;
}

		
int GUIControl::GetSupportedModes( List* modes )
{
		Message *msg = new Message( GET );
				 msg->AddString( "_type", "supported_modes" );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
				 
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;

		if ( reply->rc() != 0 ) 
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
			delete reply;
			return -1;
		}
		
		// --------------------------------
		
		int id;
		int width;
		int height;
		int mode;
		
		int count = 0;
		while ( (reply->FindInt( "_id", count, &id )) == 0 )
		{
			if ( reply->FindInt( "_width", count, &width ) == 0 )
			 if ( reply->FindInt( "_height", count, &height ) == 0 )
			  if ( reply->FindInt( "_mode", count, &mode ) == 0 )
			  {
				struct video_mode *vm = new struct video_mode;

				vm->id = id;
				vm->width = width;
				vm->height = height;
				vm->mode = mode;

				modes->add( vm );
			  }

			count += 1;
		}
	

		// --------------------------------
				
	delete reply;
	
	return 0;
}



int GUIControl::GetDesktop( int *desktop_id )
{
		Message *msg = new Message( GET );
				 msg->AddString( "_type", "desktop" );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
				 
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;


		int ans = reply->rc();

		if ( ans != 0 )
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
		}


		if ( reply->FindInt( "_id", desktop_id ) != 0 )
			ans = -1;

		delete reply;

	
	return ans;
}


int GUIControl::GetDesktopMode( int desktop_id, 
							int *video_id, 
							int *width, int *height, int *mode )
{
		Message *msg = new Message( GET );
				 msg->AddString( "_type", "desktop_mode" );
				 msg->AddInt( "_id", desktop_id );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
				 
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;


		int ans = reply->rc();

		if ( ans != 0 )
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
		}


		if ( reply->FindInt( "_id", video_id ) != 0 ) ans = -1;
		if ( reply->FindInt( "_width", width ) != 0 ) ans = -1;
		if ( reply->FindInt( "_height", height ) != 0 ) ans = -1;
		if ( reply->FindInt( "_mode", mode ) != 0 ) ans = -1;

		delete reply;

	
	return ans;
}



int GUIControl::SetDesktopDriver( int desktop_id, int video_id )
{
		Message *msg = new Message( SET );
				 msg->AddString( "_type", "desktop_driver" );
				 msg->AddInt( "_id", desktop_id );
				 msg->AddInt( "_vid", video_id );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
				 
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;


		int ans = reply->rc();

		if ( ans != 0 )
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
		}

		delete reply;
	
	return ans;
}

		
		
int GUIControl::SetMode( int video_id, int width, int height, int mode )
{
		Message *msg = new Message( SET );
				 msg->AddString( "_type", "video_mode" );
				 msg->AddInt( "_id", video_id );
				 msg->AddInt( "_width", width );
				 msg->AddInt( "_height", height );
				 msg->AddInt( "_mode", mode );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
				 
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		int ans = reply->rc();

		if ( ans != 0 )
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
		}

		delete reply;
		delete msg;
	
	return ans;
}



int GUIControl::SetDesktop( int desktop_id )
{
		Message *msg = new Message( SET );
				 msg->AddString( "_type", "desktop" );
				 msg->AddInt( "_id", desktop_id );

		Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
				 
		if ( reply == NULL )
		{
			delete msg;
			return -1;
		}

		delete msg;


		int ans = reply->rc();

		if ( ans != 0 )
		{
			if ( msg->error() != NULL )
				printf("%s%s\n","error: ", msg->error() );
		}

		delete reply;
	
	return ans;
}





