#include <types.h>
#include <stdio.h>
#include <string.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/video/VideoDriver.h>
#include <spoon/video/GUIControl.h>






VideoDriver::VideoDriver( char *name, char *description, bool receives_syncs )
: Looper( name )
{
	_name = strdup( name );
	_description = strdup( description );
	m_sync = receives_syncs;
}

VideoDriver::~VideoDriver()
{
	if ( _name != NULL ) free( _name );
	if ( _description != NULL ) free( _description );

	_modes.freeAll();
}

	  
int VideoDriver::addMode( int width, int height, int mode )
{
	struct video_mode *md = 
		(struct video_mode*)malloc(sizeof(struct video_mode));

	if ( md == NULL ) return -1;

		md->width  = width;
		md->height = height;
		md->mode  = mode;

	bool ok = _modes.add( md );

	if ( ok == false ) return -1;
	return 0;
}

	  
	  
void VideoDriver::MessageReceived( Message *msg )
{
	Message *reply;
	int width;
	int height;
	int mode;
	int rc; 
	//char *type;
		
	switch (msg->what)
	{
		case SET:
			reply = new Message( OK );

			if ( ( msg->FindInt( "_width", &width ) == 0 ) &&
			     ( msg->FindInt( "_height", &height ) == 0 ) &&
			     ( msg->FindInt( "_mode", &mode ) == 0 ) )
			{
				rc = setMode( width, height, mode );

				reply->AddInt( "rc", rc );

				if ( rc != 0 )
				reply->AddString( "_error", "driver messed up. dunno why. " );
			}
			else
			{
				reply->AddInt( "rc", -1 );
				reply->AddString( "_error", "Something was missing: "
								"_width, _height, _mode." );
				
			}
				
			msg->SendReply( reply );
			delete reply;
			break;
			
		case GET:
			get_message( msg );
			break;
			
		default:
			Looper::MessageReceived( msg );
			break;
	}
}

void VideoDriver::PulseReceived( Pulse *pulse )
{

	switch ( (*pulse)[0] )
	{
		case SYNC:
			sync( (int)((*pulse)[1]), (int)((*pulse)[2]), 
				  (int)((*pulse)[3]), (int)((*pulse)[4]) );

			break;
			
		default:
			Looper::PulseReceived( pulse );
			break;
	}
		
}
       
// -------------------------------------------------------------------------

int VideoDriver::get_lfb( Message *msg )
{
	int width;
	int height;
	int depth;
			
	void *LFB = getLFB( &width, &height, &depth );

	if ( LFB == NULL ) 
	{
		msg->SendRC( -1, "driver couldn't return an LFB" );
		return -1;
	}

	int sid = -1;//smk_share_memory( msg->source_pid(), LFB );

	if ( sid < 0 ) 
	{
		msg->SendRC( -1, "unable to share memory." );
		return -1;
	}

	// ----------------------------------------------
	

	Message *reply = new Message( OK );
			 reply->AddInt( "rc", 0 );
			 reply->AddInt( "_width", width );
			 reply->AddInt( "_height", height );
			 reply->AddInt( "_depth", depth );
			 reply->AddInt( "_sid", sid );
			 
		msg->SendReply( reply );

	delete reply;

	return 0;
}

	  
int VideoDriver::get_message( Message *msg )
{
	char *type;

	if ( msg->FindString( "_type", (const char**)&type ) != 0 )
	{
		msg->SendRC( -1, "could not find type." );
		return -1;
	}

	if ( strcmp( type, "lfb" ) == 0 )
	{
		return get_lfb( msg );
	}
	else
	{
		msg->SendRC( -1, "unknown GET request. Try: lfb.");
	}
			
	
	return 0;
}

// -------------------------------------------------------------------------


int VideoDriver::Run()
{
	int ok;

	ok = init();
	if ( ok != 0 ) return ok;

	ok = install();
	if ( ok != 0 ) return ok;
	
		ok = Looper::Run();
	
	ok = uninstall();
	if ( ok != 0 ) return ok;

	ok = shutdown();
	if ( ok != 0 ) return ok;

	return 0;
}

int VideoDriver::install()
{
	Message *msg = new Message( REGISTER );

		msg->AddString( "_type",  "video_driver" );
		msg->AddString( "_name",  _name );
		msg->AddString( "_description",  _description );
		msg->AddInt( "_port",  Port() );
		msg->AddBool( "_sync",  m_sync );

		for ( int i = 0; i < _modes.count(); i++ )
		{
			struct video_mode *mode = (struct video_mode*)_modes.get( i );
	
			msg->AddInt( "_width" , mode->width );
			msg->AddInt( "_height", mode->height );
			msg->AddInt( "_mode"  , mode->mode );
		}
	// Finished crafting message. Send and receive.

	Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
	delete msg;

	if ( reply == NULL ) return -1;

	if ( reply->what != OK )	// gui_server didn't understand the message? 
	{
		delete reply;
		return -1;
	}
	
	if ( reply->rc() != 0 )
	{
		char *error = reply->error();
		if ( error != NULL )
			printf("%s%i,%s\n","VideoDriver registration error: ",
							reply->rc(),error);
		else
			printf("%s%i\n","VideoDriver registration error: ",
							reply->rc() );

		delete reply;
		return -1;
	}

	if ( reply->FindInt("_id", &_id ) != 0 )
	{
		delete reply;
		return -1;
	}

	printf("%s%i\n","video driver installed with: ", _id );
	return 0;
}

int VideoDriver::uninstall()
{
	Message *reply = NULL;
	Message *msg = new Message( DEREGISTER );

	msg->AddString( "_type",  "video_driver" );
	msg->AddInt( "_id",  _id );

		reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );

	delete msg;

	if ( reply == NULL ) return -1;

	if ( reply->what != OK )
	{
		delete reply;
		return -1;
	}

	int rc = reply->rc();
	delete reply;
	return rc;
}

	   
   


