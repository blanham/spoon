#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <spoon/sysmsg.h>
#include <spoon/support/String.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/app/Application.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Window.h>

#include <spoon/base/Pulser.h>

// ****************************************************


// ****************************************************

Window::Window( Rect frame, const char *title, unsigned int flags )
: Looper( "window" ), 
  _application(NULL),
  m_frame( frame ), 
  m_sid(-1), 
  m_wid(-1),
  m_buffer(NULL), 
  m_focus(false), 
  _hidden(true),
  _initialShow(true),
  _active_view( NULL ), 
  _bgCol( 0x707070 ),
 m_flags( flags )
{
   m_title = strdup(title);
	
   m_pulser = NULL;
   m_pulseRate = 0;			// Default pulse rate of nothing. 

   m_did = 0;
		
   if ( application != NULL ) 
   {
		   application->lock();
		   application->AddWindow( this );
		   application->unlock();
   }

   Register();
}

Window::~Window()
{
   if ( _application != NULL ) 
   {
		 Application *app = _application;
		   app->lock();
		   app->RemoveWindow( this );
		   app->unlock();
   }

   
   while ( CountChildren() > 0 )
   {
	 View* child = RemoveChild(0);
	 delete child;
   }
   
   Deregister();

   if ( m_pulser != NULL )
   {
	   m_pulser->Terminate( true );
	   delete m_pulser;
   }

   free( m_title );
}


// ***************************************************

Rect Window::Frame() 
{ 
  return m_frame; 
}
	 
Rect Window::Bounds() 
{  
  return Rect(0,0,m_frame.Width()-1,m_frame.Height()-1); 
}

// ****************************************************
	
bool Window::ResizeTo( int width, int height )
{
	// within reason.
	if ( (width < 5) || (height < 5) ) return false;

	Rect newFrame = Frame();
		 newFrame.right  = newFrame.left + width - 1;
		 newFrame.bottom = newFrame.top + height - 1;
	
		 // Send resize request.
		Message *msg = new Message(RESIZE);
				 msg->AddInt( "_id", m_wid ); 
				 msg->AddRect( "_rect", newFrame );

		 Message *reply = Messenger::SendReceiveMessage( "gui_server",  m_did, msg );

		 if ( reply == NULL )
		    {
		      delete msg;
		      return false;
		    }
		delete msg;  

	if ( reply->rc() != 0 )
	{
		delete reply;
		return false;
	}

	// Good reply.. let's get the information.

	int bad = 0;
	int new_sid;
		if ( reply->FindInt( "_sid", &new_sid ) != 0 ) bad = -1;
		if ( reply->FindRect( "_rect", &newFrame ) != 0 ) bad = -1;
	delete reply;

	if ( bad != 0 ) return -1;

	// --------- Release what used to be.
	
		if ( m_sid >= 0 ) smk_release_shmem( m_sid );
		m_buffer 	= NULL;
		m_sid 		= new_sid;
		m_frame 	= newFrame;

	// Accept the GUI memory and everything.

	

	int tmp_pages;
	unsigned int tmp_flags;

	if ( smk_request_shmem( m_sid, (void**)&m_buffer, &tmp_pages, &tmp_flags) != 0 )
	{
		// Freak out...
		exit(-1);
	}
	
	// ....................
	

	Draw(Bounds());
	Sync();
	
	// We now have our GUI buffer, size and ID.
   return true;
}


bool Window::MoveTo( int x, int y )
{
	// Send resize request.
	Message *msg = new Message(MOVE);
			 msg->AddInt( "_id", m_wid ); 
			 msg->AddInt( "_x",  x ); 
			 msg->AddInt( "_y",  y );

		 Message *reply = Messenger::SendReceiveMessage( "gui_server", m_did, msg );
	     if ( reply == NULL )
		    {
		      delete msg;
		      return false;
		    }
		delete msg;  

	int rc = reply->rc();

	if ( rc != 0 )
	{
		delete reply;
		return false;
	}

	// Okay.. our move was granted. Let see what we were given.

	int bad = 0;
		if ( reply->FindRect( "_rect", &m_frame ) != 0 ) bad = -1;
	delete reply;

	if ( bad != 0 ) return false;

	return true;
}

// ************** SHOW/HIDE ***************************

int Window::Show()
{
   if ( _initialShow == true  )
   {
		  _initialShow = false;
   		  Resume();
   }
   
   Draw( Bounds() );

   Messenger::SendPulse( "gui_server", m_did, SHOW, m_wid, 1 );
   _hidden = false;


	  if ( m_pulseRate != 0 )
	  {
		m_pulser = new Pulser( this, m_pulseRate );
		m_pulser->Resume();
	  }


   return 0;
}

int Window::Hide()
{
	if ( m_pulser != NULL )
	{
		m_pulser->Terminate();
		delete m_pulser;
		m_pulser = NULL;
	}
		
	Messenger::SendPulse( "gui_server", m_did, HIDE, m_wid, 0 );
   _hidden = true;
   return 0;
}

bool Window::IsHidden()
{
	return _hidden;
}

// ************* GUI SERVER ***************************

int Window::Register()
{
	Message *msg = new Message(REGISTER);

			 msg->AddString( "_type", "window" ); 
			 msg->AddInt( "_port", Port() );
			 msg->AddString( "_title", m_title ); 
			 msg->AddRect( "_rect", Frame() );
			 msg->AddBool( "_visible", false );
			 msg->AddInt( "_flags", m_flags );
		

	 Message *reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
     if ( reply == NULL )
	    {
	      delete msg;
	      return -1;
	    }
	delete msg;  

	if ( reply->rc() != 0 )
	{
		delete reply;
		return -1;
	}

	// Good reply.. let's get the information.

	int bad = 0;
	
	if ( reply->FindInt( "_id", &m_wid ) != 0 ) bad = -1;
	if ( reply->FindInt( "_sid", &m_sid ) != 0 ) bad = -1;
	if ( reply->FindInt( "_did", &m_did ) != 0 ) bad = -1;
	if ( reply->FindRect( "_rect", &m_frame ) != 0 ) bad = -1;

	delete reply;
	

	// Accept the GUI memory and everything.

	int tmp_pages;
	unsigned int tmp_flags;
			
	if ( smk_request_shmem( m_sid, (void**)&m_buffer, &tmp_pages, &tmp_flags ) != 0 )
			bad = -1;
	
	// We now have our GUI buffer, size and ID.
	
	if ( bad != 0 ) return -1;

	// Registered with desktop port =  m_did
	
   return 0;
}

int Window::Deregister()
{
	Message *msg = new Message( DEREGISTER );
			 msg->AddInt( "_id", m_wid );

	Message *reply = Messenger::SendReceiveMessage( "gui_server", m_did, msg );
    if ( reply == NULL )
	{
	    delete msg;
	    return -1;
	}
	delete msg;  

	// Check response.. not that we care, really.

	int ans = reply->rc();
	delete reply;

	// And... breathe...
	
	if ( m_sid >= 0 ) smk_release_shmem( m_sid );
	m_buffer = NULL;
	m_sid = -1;
	m_did = 0;

	return ans;
}

// ***************************************************


void Window::Sync( int left, int top, int right, int bottom )
{
	if ( (left == 0) && (top == 0) && (right == -1) && (bottom == -1) )
		Messenger::SendPulse( "gui_server", m_did, SYNC, m_wid, 0,0, m_frame.Width() - 1, m_frame.Height() - 1 );	
	else
		Messenger::SendPulse( "gui_server", m_did, SYNC, m_wid, left, top, right, bottom );
}

// *****************************************************

void Window::Focus( bool focus )
{
	// Blah.. whatever. I don't care. I'm just a default window.
}

bool Window::IsFocus()
{
	return m_focus;
}


// *****************************************************

uint32 Window::BackgroundColour() 
{ 
	return _bgCol; 
}

void Window::SetBackgroundColour( uint32 col ) 
{ 
   _bgCol = col; 
}


/** Overrides the default pulse rate of 500 ms. This sets
 * the global pulse rate. All Views are affected.
 *
 * \param milliseconds The number of milliseconds between pulses.
 * \return The old number of milliseconds between pulses.
 */
unsigned int Window::SetPulseRate( unsigned int milliseconds )
{
	m_pulseRate = milliseconds;
		
	if ( IsHidden() == true ) return milliseconds;

	if ( (milliseconds == 0) )
	{
		if ( m_pulser != NULL )
		{
			m_pulser->Terminate();
			delete m_pulser;
			m_pulser = NULL;
		}
		return milliseconds;
	}
	
	if ( m_pulser == NULL )
	{
		m_pulser = new Pulser( this, milliseconds );
		m_pulser->Resume();
	}
	else
	{
		m_pulser->setRate( milliseconds );
	}

	return milliseconds;
}

/** This is a virtual method which is called whenever the
 * Window receives a PULSE message from within the
 * application (same pid).
 *
 * You can override it to do whatever you want but perhaps
 * you should be overriding a View's Pulsed() method?.
 *
 * If you do decide to override, please don't forget to
 * call Window::Pulsed() or else none of the children views
 * will get their Pulsed.
 */
void Window::Pulsed()
{
	lock();

	for ( int i = 0; i < m_pulseViews.count(); i++ )
	{
		View *view = (View*)m_pulseViews.get(i);
		view->Pulsed();
	}

	unlock();
}

/** If you override this, please remember to
 * call DrawViews() and Sync() there after.
 */
void Window::Draw( Rect frame )
{
	FillRect( frame, _bgCol );
	DrawViews( frame );
	Sync();
}

/** Please be sure to call this should you ever
 * override the default Draw() method.
 */
void Window::DrawViews( Rect frame )
{
	/// \todo Only update the requested frame.
   lock();
   for ( int i = 0; i < CountChildren(); i++ )
    {
      View *view = ChildAt(i);
            view->Draw( view->Bounds() );
            view->DrawChildren( view->Bounds() );
    }
   unlock();
}

// ******************************************************



bool Window::AddChild( View *view )
{
   bool ans = _views.add( view );
   view->_window = this;
   view->AttachedToWindow();

   if ( (view->getFlags() & PULSE_NEEDED) != 0 ) AddPulseChild( view );

   return ans;
}


View *Window::ChildAt( int i )
{
  View *ans = (View*)_views.get(i);
  return ans;
}

View *Window::ChildAt( int x, int y )
{
  lock();
  for ( int i = CountChildren() - 1; i >= 0; i-- )  
  {
     View *view = ChildAt(i);

     if ( view->Frame().Contains( x,y ) )
     {
	   unlock();
	   return view;
  	 }
  }
  unlock();

  return NULL;
}

int Window::CountChildren()
{
	int ans = _views.count();
	return ans;
}


View* Window::RemoveChild( int i )
{
	View *target = (View*)_views.get( i );
	_views.remove( target );
	return target;
}


bool Window::RemoveChild( View *view )
{
	bool ans =  _views.remove( view );
	if ( ans )
	{
   	   if ( (view->getFlags() & PULSE_NEEDED) != 0 ) RemovePulseChild( view );
       view->_window = NULL;
       view->DetachedFromWindow();
	}
	return ans;
}



// ***********************************************

void Window::SetActiveView( View *view )
{
  if ( _active_view == view ) return;
  if ( _active_view != NULL ) 
    _active_view->SetActive( false );
    
  _active_view = view;

  if ( _active_view != NULL ) 
	    _active_view->SetActive( true );
}

View* Window::ActiveView() 
{ 
	return _active_view; 
}



// **********************************************************


void Window::MouseEvent( int what, int x, int y, unsigned int buttons )
{

			// Backwards, as added.
  lock();
  for ( int i = CountChildren() - 1; i >= 0; i-- )  
  {
     View *view = ChildAt(i);

        if ( view->Frame().Contains( x,y ) )
         {
		   int nx = x - view->Frame().left;
		   int ny = y - view->Frame().top;

		   SetActiveView( view );
		   view->MouseEvent( what, nx, ny, buttons );
		   break;
	  	 }
  }
  unlock();
  
}


void Window::KeyEvent( uint32 what, 
			uint32 code, 
			uint32 ascii, 
			uint32 modifiers )
{
   if ( ActiveView() == NULL ) return;
   
   ActiveView()->KeyEvent( what, code, ascii, modifiers );
}



// **********************************************************


void Window::PutPixel( int x, int y, uint32 col )
{
	if ( m_buffer == NULL ) return;
	
     if ( (y>=0) && (y < m_frame.Height()) &&
	      (x>=0) && (x < m_frame.Width()) )
              m_buffer[ y * m_frame.Width() + x ] = col;
}

uint32 Window::GetPixel( int x, int y )
{
	if ( m_buffer == NULL ) return 0;
		
      if ( (y>=0) && (y < m_frame.Height()) &&
	       (x>=0) && (x < m_frame.Width()) )
	            return m_buffer[ y * m_frame.Width() + x ];

	 return 0;
}



/** Should only be called by a View when it's AttachedToWindow
 * and it has a PULSE_NEEDED flag.
 */
bool Window::AddPulseChild( View* view )
{
	if ( m_pulseViews.indexOf( view ) >= 0 ) return false;
	return m_pulseViews.add( view );
}

/** Should only be called by the View or self when it's removing a child
 * which is registered with PULSE_NEEDED
 */
bool Window::RemovePulseChild( View *view )
{
	return m_pulseViews.remove( view );
}


// **********************************************************



void Window::QuitRequested()
{
}


void Window::PulseReceived( Pulse *pulse )
{
  Rect oldFrame;
   uint32 d1,d2,d3,d4,d5,d6;
   d1 = (*pulse)[0];
   d2 = (*pulse)[1];
   d3 = (*pulse)[2];
   d4 = (*pulse)[3];
   d5 = (*pulse)[4];
   d6 = (*pulse)[5];



  switch (d1)
  {
	 case PULSE:
		  if ( smk_getpid() != pulse->source_pid ) break; // Ignore the daemons.
		  Pulsed();
		  break;

     case FOCUS:
          m_focus = (d2 == 1);
     	  Focus( m_focus );
		  break;

     case MOUSE_DOWN:
     case MOUSE_UP:
     case MOUSE_MOVED:
           MouseEvent( d1, d2, d3, d4 );
		   break;
	 
     case KEY_DOWN:
     case KEY_UP:
           KeyEvent( d1, d2, d3, d4 ); 
     	   break;

     case DRAW:
     	  Draw( Rect(d2,d3,d4,d5) );
		  Sync();
	  	  break;

     case MOVE:
           oldFrame = m_frame;
     	   m_frame.left   += d2;
     	   m_frame.right  += d2;
     	   m_frame.top    += d3;
     	   m_frame.bottom += d3;
	   	   WindowMoved( Frame() );
		   break;

     default:
       Looper::PulseReceived( pulse );
       break;
  }
}




