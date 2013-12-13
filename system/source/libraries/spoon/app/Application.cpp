#include <types.h>
#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Looper.h>
#include <spoon/app/Application.h>
#include <spoon/interface/Window.h>


/** The global Application variable. Always set to the running Application. */
Application *application = NULL;

/** The constructor is responsible for setting the global application variable 
 * to this, saving the given name and for creating the IPC port 0.
 *
 * \param name The name of the application.
 */
Application::Application( const char *name )
: _app_terminated(false)
{
   application = this;
   app_name.SetTo( name );
   smk_create_port( 0, smk_gettid() );
}

/** The destructor releases IPC port 0, locks the Application, runs through
 * the internal window list and calls Quit() on whichever Windows might be
 * hanging around.
 */
Application::~Application()
{
	smk_release_port( 0 );
	
	/// \todo This could be cleaner but it should let the Windows free themselves.
	
	lock();
	for ( int i = 0; i < _window_list.count(); i++ )
	{
		Window *win = WindowAt(i);
		if ( win != NULL ) win->Quit();
	}
	unlock();
	
	// Let the Windows delist themselves.
	while ( CountWindows() > 0 )
	{
		smk_release_timeslice();
		smk_release_timeslice();
		smk_release_timeslice();
	}

	smk_release_timeslice();
	smk_release_timeslice();
	smk_release_timeslice();
	
}

// *************** Window List ************************

/** Adds a Window to this Application. This method also
 * sets up the Window so that it knows who its Application
 * is. 
 * 
 * \param win The Window to add.
 * \return true on a successful Add.
 */
bool Application::AddWindow( Window *win )
{
	bool ans = _window_list.add( win );
 	/// \todo Re-evaluate the point of this considering that application is global.
	if ( ans ) win->_application = this;
	return ans;
}

/** Returns the window at the given index. 
 *
 * \param index The index of the requested Window.
 * \return the Window at index or NULL if there's nothing there.
 */
Window* Application::WindowAt( int index )
{
	Window *win = (Window*) _window_list.get( index );
	return win;
}

/** \return The number of Windows registered with the Application */
int Application::CountWindows()
{
	int ans = _window_list.count();
	return ans;
}

/** \return true if the Application found and removed the given Window */
bool Application::RemoveWindow( Window *win )
{
	bool ans = _window_list.remove( win );
	if ( ans == true ) win->_application = NULL;
	return ans;
}

// *************** Pre and post hooks *****************


/** This is a virtual method which is called just before the IPC loop
 * begins. 
 *
 * \return 0 if the Application can continue to load and run. Anything
 * 	       else will cause the Application to abort loading and return
 * 	       to the calling function.
 */
int Application::Init()
{
	return 0;
}

/** This is a virtual method which is called just after the IPC
 * loop finishes (usually caused by a call to Quit() or receiving
 * a QUIT_REQUESTED message/pulse)
 */
void Application::Shutdown()
{
}


// *************** Main Message Loop ****************** 


/** This is the main IPC loop of the Application which is
 * exactly like the Looper's IPC loop except that it calls
 * the virtual methods Init() and Shutdown() at the entry
 * and exit of the method. 
 *
 * \return It should return 0 on successful exit.
 */
int Application::Run()
{
   int source_pid;
   int source_port;
   int dest_port;
   uint32 d1,d2,d3,d4,d5,d6;
   int length;

   if ( Init() !=  0 ) return -1;

   while ( _app_terminated == false )
   {
      smk_go_dormant();

		// Check for pulses		
      if ( smk_recv_pulse( &source_pid, &source_port, &dest_port,
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

   
   Shutdown();
   return 0;
}


// *******************************************************************

/** This is a virtual method which is called whenever the Application
 * receives a request to quit, either by Quit() or by receiving a
 * QUIT_REQUESTED pulse or message.
 *
 * You would want to override this method if you want to do some stuff 
 * before the Application IPC loop exits.
 */
void Application::QuitRequested()
{
}

// *******************************************************************

/** Override this method if you want to handle any specific messages
 * sent to this Application.
 *
 * \warning Do not delete the msg parameter. It does not belong to you.
 */
void Application::MessageReceived( Message *msg )
{
}

/** Override this method if you want to handle any specific pulses
 * sent to this Application.
 *
 * \warning Do not delete the pulse parameter. It does not belong to you.
 */

void Application::PulseReceived( Pulse *pulse )
{
	switch ( (*pulse)[0] )
	{
		case QUIT_REQUESTED:
			QuitRequested();
			_app_terminated = true;
			break;
	}
}



	 
