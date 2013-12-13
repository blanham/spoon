#include <stdio.h>
#include <spoon/sysmsg.h>
#include <spoon/interface/Window.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Region.h>

#include <Desktop.h>

//#define BGCOLOR		0x606080
//#define BGCOLOR		0x402020
#define BGCOLOR		0x202040

namespace GUI {

Desktop::Desktop()
{
	movingRect 		= NULL;
	movingWindow 	= NULL;
	desktopWindow 	= NULL;
	m_double_buffer = NULL;
	m_buffering 	= false;

	drawing_lock 	= 0;

	initScreen();
	drawDesktop();
	showCursor( 400, 300, 0 );
}

Desktop::~Desktop()
{
	if ( movingRect != NULL ) delete movingRect;
	if ( movingWindow != NULL ) movingWindow = NULL;
	if ( desktopWindow != NULL ) desktopWindow = NULL;
	if ( m_double_buffer != NULL ) free( m_double_buffer );
}


Rect Desktop::Bounds()
{
	return Rect(0,0,m_width-1, m_height-1);
}

// ---------------------------------------------------


bool Desktop::setDesktopWindow( Window *win )
{
	if ( desktopWindow != NULL ) 
	{
		printf("%s%x, %x\n","add: ", desktopWindow, win );
		add( win );	// Sorry buddy...
	}
	else
	{
		printf("%s%x, %x\n","desktopWindow: ", desktopWindow, win );
		desktopWindow = win;
	}

	return true;
}

// ---------------------------------------------------


bool Desktop::drawDesktop()
{
	m_buffering = true;	// Buffering on...
		
	// Background colour
	FillRect( 0,0, m_width, m_height, BGCOLOR );

	// desktop window.
	
	if ( desktopWindow != NULL )
	{
		syncWindow( desktopWindow,
					0,0,
					desktopWindow->Frame().Width() - 1,
					desktopWindow->Frame().Height() - 1 );
	}
	
	// windows.
	for ( int i = 0; i < count(); i++ )
	{
		Window *win = (Window*)get(i);
		if ( win->isVisible() == false ) continue;

		syncWindow( win, 
					0,0, 
					win->Frame().Width()-1,	
					win->Frame().Height()-1 );

		drawFrame( win );
	}

	m_buffering = false; // Buffering off...

	// Flip..
	for ( int i = 0; i < (m_width * m_height); i++ )
		m_lfb[ i ] = m_double_buffer[ i ];

	
	return true;
}



// ****************************************************

bool Desktop::drawFrame( Window *win )
{
	if ( (win->flags() & WINDOW_HAS_NOFRAME) == WINDOW_HAS_NOFRAME ) return true; 
	

	uint32 col_offset = 0;
	if ( isFocus(win) ) col_offset = 0x222200;
	
	

	if ( (win->flags() & WINDOW_HAS_BORDER) == WINDOW_HAS_BORDER )
	{
		Rect frame = win->Frame();
	
			frame.left -= 1;
			frame.top  -= 1;
			frame.right += 1;
			frame.bottom += 1;
			DrawRect( frame, 0x40 + col_offset );

			frame.left -= 1;
			frame.top  -= 1;
			frame.right += 1;
			frame.bottom += 1;
			DrawRect( frame, 0x101070 + col_offset );
			
			frame.left -= 1;
			frame.top  -= 1;
			frame.right += 1;
			frame.bottom += 1;
			DrawRect( frame, 0x202080 + col_offset );

		
		return true;
	}
	
		
	if ( (win->flags() & WINDOW_HAS_TITLE) == WINDOW_HAS_TITLE ) 
	{
		Rect frame = win->Frame();
	
			frame.left -= 1;
			frame.top  -= 1;
			frame.right += 1;
			frame.bottom += 1;
			DrawRect( frame, 0x30 + col_offset );

			for ( int i = 0; i < 3; i++ )
			{
				frame.left -= 1;
				frame.right += 1;
				frame.bottom += 1;
				DrawLine( frame.left, frame.top, 
						  frame.left, frame.bottom, 0x303070 + col_offset );
				DrawLine( frame.right, frame.top, 
						  frame.right, frame.bottom, 0x303070 + col_offset );
				DrawLine( frame.left, frame.bottom, 
						  frame.right, frame.bottom, 0x303070 + col_offset );
			}


			frame.top = win->Frame().top - 19;
			frame.bottom = win->Frame().top - 2;
			
			// Add the curve at the top...
			for ( int i = 0; i < 4; i++ )
			{
				int off = 0;

				if ( i == 3 )  off = 1;
				if ( i == 2 )  off = 1;
				if ( i == 1 )  off = 2;
				if ( i == 0 )  off = 4;

				// Save the background around the edges..
				for ( int j = 0; j < off; j++ )
				{
					uint32 col;
					
					col = GetPixel( frame.left + j, frame.top + i );
					PutPixel( frame.left + j, frame.top + i, col );

					col = GetPixel( frame.right - j, frame.top + i );
					PutPixel( frame.right - j, frame.top + i, col );
				}
				
				DrawLine( frame.left + off, frame.top + i, 
						  frame.right - off, frame.top + i, 
						  0x303070 + col_offset );
			}

			// Fill the rest of the top.
			frame.top += 4;
			FillRect( frame, 0x303070 + col_offset );
			frame.top -= 4;	// An fix correction.
	

			
			DrawString( frame.left + 4, frame.top + 2, win->title(), 0x5050A0 + col_offset );

			// Draw the close button.
			
			frame = win->Frame();
			
			frame.bottom 	= frame.top - 2;
			frame.top  		-= 17;
			frame.left 		= frame.right - 22;
			frame.right 	-= 2;

			DrawRect( frame, 0x5050A0 + col_offset );
	
			DrawLine( frame.left + 2, frame.top + 2, frame.right - 2, frame.bottom - 2, 0x5050A0 + col_offset );
			DrawLine( frame.right - 2, frame.top + 2, frame.left + 2, frame.bottom - 2, 0x5050A0 + col_offset );
			
			return true;
	}


	
	return true;
}


void Desktop::adjustFrameForBorder( Window *win, Rect &frame )
{
	if ( (win->flags() & WINDOW_HAS_NOFRAME) == WINDOW_HAS_NOFRAME ) return; 

	if ( (win->flags() & WINDOW_HAS_BORDER) == WINDOW_HAS_BORDER )
	{
		frame.left -= 3;
		frame.top  -= 3;
		frame.right += 3;
		frame.bottom += 3;
		return;
	}
	
		
	if ( (win->flags() & WINDOW_HAS_TITLE) == WINDOW_HAS_TITLE ) 
	{
		frame.left -= 4;
		frame.top  -= 19;
		frame.right += 4;
		frame.bottom += 4;
		return;
	}


}


// --------------------------------------------------

bool Desktop::actionAt( uint32 what,
						int x, int y, unsigned int buttons, 
						Window **window, int *action )
{
	// Short-circuit some stuff if in unusual states.
	
	if ( (what == MOUSE_UP) && (buttons == 0) && ( movingRect != NULL ) )
	{
		// Someone released the mouse buttons while moving.
		*window = movingWindow;
		*action = ACTION_MOVE_FINISH;
		return true;
	}
	

	if ( (what == MOUSE_DOWN) && ( movingRect != NULL ) )
	{
		// Someone pressed more than 1 mouse button down while moving a window.
		return false;
	}
		
		

	// ------------------------------------------------------------
	
	bool transparentPassThru = false;
	
	for ( int i = count()-1; i >= 0; i--)
	{
		Window *win = (Window*)get(i);
		if ( win->isVisible() == false ) continue;			// Not visible. 

		if ( win->Frame().Contains( x, y ) ) 				// Inside the window proper.
		{
			// Pass through if transparent.
			if ( win->isTransparent() == true )
			{
			// Is it on a transaparent pixel.
				uint32 colour = (win->buffer())[ (y-win->Frame().top) * win->Frame().Width() + (x-win->Frame().left) ];
				if ( colour == TRANSPARENT )
				{
					transparentPassThru = true;
					continue;		// Yes. Pass through.
				}
			}

			// Nope. It needs to go to this window.
			*window = win;

			if ( transparentPassThru == true ) *action = ACTION_PASSTHRU_TRANS;
								  	 	  else *action = ACTION_PASSTHRU;
				
			return true;
		}


		if ( what == MOUSE_MOVED ) continue;				// No point in checking frames if no "event" occured.
		if ( what == MOUSE_UP ) continue;					// Also, no point if MOUSE_UP isn't inside a frame while not moving.
		


		if ( (win->flags() & WINDOW_HAS_NOFRAME) == WINDOW_HAS_NOFRAME ) continue;	// No frame, anyway.
		
		if ( (win->flags() & WINDOW_HAS_BORDER) == WINDOW_HAS_BORDER )	// A simple frame. 
		{
			Rect frame = win->Frame();

				frame.left -= 3;
				frame.top  -= 3;
				frame.right += 3;
				frame.bottom += 3;

			if ( frame.Contains( x, y ) == false ) continue;


			*window = win;
			*action = ACTION_MOVE_START;
			movingRect = new Rect( frame );
			return true;
		}
		
		if ( (win->flags() & WINDOW_HAS_TITLE) == WINDOW_HAS_TITLE )	// A title frame.
		{
			Rect frame = win->Frame();

				frame.left -= 4;
				frame.top  -= 19;
				frame.right += 4;
				frame.bottom += 4;

			if ( frame.Contains( x, y ) == false ) continue;

			// Check to see if closure requested.


			frame = win->Frame();
			
				frame.bottom 	= frame.top - 2;
				frame.top  		-= 17;
				frame.left 		= frame.right - 22;
				frame.right 	-= 2;

			if ( frame.Contains( x, y ) == true )
			{
				*window = win;
				*action = ACTION_CLOSE;
				return true;
			}

			

			*window = win;
			*action = ACTION_MOVE_START;
			movingRect = new Rect( frame );
			return true;
		}
	}
	

	// No active windows were found. Let's check the desktop
	// if it's installed.
	
	if ( desktopWindow == NULL ) return false;
		
	if ( desktopWindow->Frame().Contains( x, y ) == false ) return false;
	
	// Forget if transparent pixel.
	if ( desktopWindow->isTransparent() == true )
	{
		uint32 position = (y-desktopWindow->Frame().top) * desktopWindow->Frame().Width() + (x-desktopWindow->Frame().left);
		uint32 colour = (desktopWindow->buffer())[ position  ];
		if ( colour == TRANSPARENT ) return false;
	}


	*window = desktopWindow;
	*action = ACTION_DESKTOP;
	return true;
}

// ---------------------------------------------------

Window *Desktop::windowBy( int pid, int wid )
{
	for ( int i = count()-1; i >= 0; i--)
	{
		Window *win = (Window*)get(i);
		if ( (win->pid() == pid) && 
			 (win->wid() == wid ) ) return win;
	}

	if ( desktopWindow != NULL )
	{
		if ( (desktopWindow->pid() == pid) &&
			 (desktopWindow->wid() == wid) )
				return desktopWindow;
	}

	return NULL;
}


// ------------------------------------------------------


int Desktop::syncWindow( Window *win,
					   int left, int top,
					   int right, int bottom )
{
	// Let's start validating and cleaning this request.
	if ( win->isVisible() == false ) return 0;

	// valid co-ordinates
	if ( right <= left ) return -1;
	if ( bottom <= top ) return -1;
	
	// Save a few calls.
	Rect windowFrame = win->Frame();
	
	// shrink to support the window if too large.
		
		if ( right >= windowFrame.Width() )   right = windowFrame.Width() - 1;
		if ( bottom >= windowFrame.Height() ) bottom = windowFrame.Height() - 1;
		if ( left < 0 ) left = 0;
		if ( top < 0 ) top = 0;

		// Nothing to hide..
		if ( isFocus( win ) )
				return subSync( win, left, top, right, bottom );
				
	
	// Adjust to screen quickly.
	Rect screenFrame( windowFrame.left + left, 
					  windowFrame.top  + top, 
					  windowFrame.left + right, 
					  windowFrame.top  + bottom ); 
	
	/// \todo Precalculate this and only update when the windows move.
	// Exclude invisible parts.
	Region region( screenFrame );
	
		for ( int i = count()-1; i >= 0; i--)
		{
			Window *tmp = (Window*)get(i);
			if ( tmp == win ) break;
			// Okay, transparent and invisible windows don't count
			if ( tmp->isVisible() == false ) continue;
			if ( tmp->isTransparent() == true ) continue;
			

			Rect tempRect = tmp->Frame();
			
			adjustFrameForBorder( tmp, tempRect );
			
			region.Exclude( tempRect );
		}
		
	// Re-adjust to the window.
		region.OffsetBy( - windowFrame.left, - windowFrame.top ); 
		
		for ( int i = 0; i < region.CountRects(); i++ )
		{
			Rect *tmp = region.RectAt(i);

			subSync( win, 
					 tmp->left, tmp->top,
					 tmp->right, tmp->bottom );
		}
	
		
	return 0;
}

int Desktop::subSync( Window *win,
					   int left, int top,
					   int right, int bottom )
{
			// adjust to screen.
		int screen_left 	= win->Frame().left; 
		int screen_top 		= win->Frame().top; 
	

	// Transpose onto real screen.

	uint32 *win_buffer = win->buffer();
	int 	 win_width = win->Frame().Width();
	bool	transparent= win->isTransparent();
	
	
	for ( int y = top; y <= bottom; y++ )
	{
		int real_y = screen_top + y;

		if ( real_y < 0 ) continue;
		if ( real_y >= m_height ) break;
			
		for ( int x = left; x <= right; x++ )
		 {
			int real_x = screen_left + x;

			if ( real_x < 0 ) continue;
			if ( real_x >= m_width ) break;
			
			
			// A PutPixel
			uint32 colour = win_buffer[ y * win_width + x ];

			// Determine behaviour based on pixel colour. (For
			// those special colours.)
			switch (colour)
			{
				case TRANSPARENT:	// Don't draw transparent if required.
				    if ( transparent == true ) break;
				default:
					if ( m_buffering == true )
						m_double_buffer[ real_y * m_width + real_x ] = colour;
					else
						m_lfb[ real_y * m_width + real_x ] = colour;
					break;
			}
			
		 }
	}
		
		
	return 0;
}


int Desktop::showWindow( int pid, int id, bool visible )
{
	Window *win = windowBy( pid, id );
	if ( win == NULL ) return -1;
	
	if ( win->isVisible() == visible ) return 0;
	
	win->setVisible( visible );

	hideCursor( prev_x, prev_y, prev_buttons );
	drawDesktop();
	showCursor( prev_x, prev_y, prev_buttons );
	return 0;
}

// ------------------------------------------------------

bool Desktop::focus( Window *win )
{
	if ( win == desktopWindow )
	{
		int i = 0;
			i = 10 / i;
		// Crash... bad code.
		/// \todo Remove one day.
	}

	Window *old_window = (Window*)get( count() - 1 );
	if ( old_window != NULL )
	{
		if ( old_window == win ) return true;
	}

	if ( remove( win ) != true ) return false;
	if ( add( win ) == false )
	{
		/// \todo actually freak out.
		return false;
	}


	hideCursor( prev_x, prev_y, prev_buttons );
	drawDesktop();
	showCursor( prev_x, prev_y, prev_buttons );


	// Let the windows know when they're out of focus ... or in.
	old_window->signalFocus( false );
	win->signalFocus( true );
	
	
	return true;
}

bool Desktop::isFocus( Window *win )
{
	if ( get( count() - 1 ) == win ) return true;
	return false;
}

// ------------------------------------------------------------

void Desktop::KeyPressed( uint32 what, 
				uint32 code, 
				uint32 ascii, 
				uint32 modifiers )
{
	this->modifiers = modifiers;
		
	if ( count() <= 0 ) return;
	
	Window *win = (Window*)get( count() - 1 );
	if ( win == NULL ) return;

	win->KeyPressed( what, code, code, modifiers );
}

// ------------------------------------------------------------

void Desktop::MouseEvent( uint32 what, int x, int y, unsigned int buttons )
{
	hideCursor( x, y, buttons );
	showCursor( x, y, buttons );
		
	Window *win = NULL;
	int action = 0;
	
	if ( actionAt( what, x, y, buttons, &win, &action ) == false ) return;
	
	if ( (what == MOUSE_MOVED) && 
		 ( (win->flags() & WINDOW_GETS_MOUSEMOVED) != WINDOW_GETS_MOUSEMOVED )
	   ) return;

	switch (action)
	{
		case ACTION_PASSTHRU:
			win->MouseEvent( what, x, y, buttons );
			if ( what != MOUSE_UP ) focus( win );
			break;

		case ACTION_PASSTHRU_TRANS:
			win->MouseEvent( what, x, y, buttons );
			break;

		case ACTION_DESKTOP:
			if ( what == MOUSE_DOWN ) desktopWindow->signalFocus( true );
			win->MouseEvent( what, x, y, buttons );
			break;
			
		case ACTION_MOVE_START:
			movingWindow = win;

			// Offset the moving rect by our cursor.
				movingRect->left 	= win->Frame().left - x;
				movingRect->right 	= win->Frame().right - x;
				movingRect->top 	= win->Frame().top - y;
				movingRect->bottom 	= win->Frame().bottom - y;
			
			break;

		case ACTION_MOVE_FINISH:

				movingRect->left  	+= x;
				movingRect->right 	+= x;
				movingRect->top  	+= y;
				movingRect->bottom 	+= y;
			
				movingWindow->moveTo( *movingRect );
				
				win = movingWindow;
			
			delete movingRect;
			movingRect = NULL;
			movingWindow = NULL;

			if ( isFocus( win ) )
			{
				hideCursor( prev_x, prev_y, prev_buttons );
				drawDesktop();
				showCursor( prev_x, prev_y, prev_buttons );
			}
			else focus( win );

			break;
			
		case ACTION_CLOSE:
			win->close();
			
			break;
			
	}


}

// -------------------------------------------------------------
#include "cursor.inc"


void Desktop::hideCursor( int x, int y, unsigned int buttons )
{
  
  	// Drawing over the old rectangle...
		if ( (movingRect != NULL) )
   		{
   			for ( int i = 0; i < movingRect->Width(); i++ )
			{
				int nx = movingRect->left + i + prev_x;
				PutPixel( nx , movingRect->top + prev_y, ~(GetPixel( nx, movingRect->top + prev_y )) ); 
				PutPixel( nx , movingRect->bottom + prev_y, ~(GetPixel( nx, movingRect->bottom + prev_y )) ); 
			}	

  			for ( int j = 0; j < movingRect->Height(); j++ )
			{
				int ny = movingRect->top + j + prev_y;
				PutPixel( movingRect->left + prev_x, ny, ~(GetPixel( movingRect->left + prev_x, ny )) ); 
				PutPixel( movingRect->right + prev_x, ny, ~(GetPixel( movingRect->right + prev_x, ny )) ); 
			}	

   		}
 

			 
	// Old cursor position....
    for ( int i = 0; i < 8; i++ )
      for ( int j = 0; j < 8; j++ )
		PutPixel( prev_x + i, prev_y + j, saved[i][j] );
}


void Desktop::showCursor( int x, int y, unsigned int buttons )
{
	 // Save new spot
   for ( int i = 0; i < 8; i++ )
    for ( int j = 0; j < 8; j++ )
      saved[i][j] = GetPixel( x + i, y + j );


     
   // cursor 
   for ( int i = 0; i < 8; i++ )
    for ( int j = 0; j < 8; j++ )
	{
	  uint32 col;

	  if ( buttons != 0 ) col = cursor_bitmap_down[i][j];
					 else col = cursor_bitmap_up[i][j];
			
	  if ( col != TRANSPARENT )
		PutPixel( x + i, y + j, col );
	}

   prev_x = x;
   prev_y = y;
   prev_buttons = buttons;


	  	// Drawing rectangle... again....
		if ( (movingRect != NULL) )
   		{
   			for ( int i = 0; i < movingRect->Width(); i++ )
			{
				int nx = movingRect->left + i + prev_x;
				PutPixel( nx , movingRect->top + prev_y, ~(GetPixel( nx, movingRect->top + prev_y )) ); 
				PutPixel( nx , movingRect->bottom + prev_y, ~(GetPixel( nx, movingRect->bottom + prev_y )) ); 
			}	

  			for ( int j = 0; j < movingRect->Height(); j++ )
			{
				int ny = movingRect->top + j + prev_y;
				PutPixel( movingRect->left + prev_x, ny, ~(GetPixel( movingRect->left + prev_x, ny )) ); 
				PutPixel( movingRect->right + prev_x, ny, ~(GetPixel( movingRect->right + prev_x, ny )) ); 
			}	

   		}
 


}





/* -------------------------------------------------------- */


int Desktop::deregisterWindow( int pid, int wid )
{
	Window *win = windowBy( pid, wid );
	if ( win == NULL ) return -1;

	bool redraw = win->isVisible();

	if ( win != desktopWindow ) remove( win );
					else desktopWindow = NULL;

	if ( (movingWindow == win) && (movingRect != NULL) )
	{
		delete movingRect;
		movingRect = NULL;
		movingWindow = NULL;
	}
	
	delete win;

	if ( redraw ) 
	{
		hideCursor( prev_x, prev_y, prev_buttons );
		drawDesktop();
		showCursor( prev_x, prev_y, prev_buttons );
	}
	return 0;
}
/* -------------------------------------------------------- */


int Desktop::resizeWindow( Message *msg, int pid, int wid, Rect *frame )
{
	// See if we can get the Window...
	Window *win = windowBy( pid, wid );
	if ( win == NULL ) 
		return msg->SendRC( -1, "unable to find window");
		
	// See if the window can resize.
	if ( win->resizeTo( frame->Width(), frame->Height() ) == false )
		return msg->SendRC( -1, "resizing failure." );


	// Send a successful reply.
	Message *reply = new Message(0);
			 reply->AddInt(  "rc", 0 );
			 reply->AddInt(  "_sid", win->sid() );
			 reply->AddRect( "_rect", win->Frame() );
	msg->SendReply( reply );
	delete reply;
		
	return 0;
}

/* -------------------------------------------------------- */

int Desktop::moveWindow( Message *msg, int pid, int wid, int x, int y )
{
	// See if we can get the Window...
	Window *win = windowBy( pid, wid );
	if ( win == NULL ) 
		return msg->SendRC( -1, "unable to find window");
	
	Rect currentPosition = win->Frame();

	// Fit into screen...
		int width = currentPosition.Width();
		int height = currentPosition.Height();

			if ( x + width >= m_width ) x = m_width - width - 1;
			if ( y + height >= m_height ) y = m_height - height - 1;
			if ( x < 0 ) x = 0;
			if ( y < 0 ) y = 0;

	// adjust the window position.
	
		currentPosition.left 	= x;
		currentPosition.top 	= y;
		currentPosition.right 	= x + width - 1;
		currentPosition.bottom 	= y + height - 1;
			
	// See if the window can move.
	if ( win->moveTo( currentPosition ) == false )
		return msg->SendRC( -1, "moving failure." );

	// Send a successful reply.
	Message *reply = new Message(0);
			 reply->AddInt(  "rc", 0 );
			 reply->AddRect( "_rect", win->Frame() );
	msg->SendReply( reply );
	delete reply;
		
	hideCursor( prev_x, prev_y, prev_buttons );
	drawDesktop();
	showCursor( prev_x, prev_y, prev_buttons );
	
	return 0;
}

/* -------------------------------------------------------- */

void Desktop::MessageReceived( Message *msg )
{
	int id;
	int x, y;
	Rect frame;
		
	switch ( msg->what )
	{
		case DEREGISTER:
			if ( msg->FindInt("_id", &id ) != 0 ) 
			{
				msg->SendRC( -1, "no _id found." );
				return;
			}
			
			msg->SendRC( deregisterWindow( msg->source_pid(), id ) ); 
			break;

		case RESIZE:
			if ( msg->FindInt("_id", &id ) != 0 ) 
			{
				msg->SendRC( -1, "no _id found." );
				return;
			}

			if ( msg->FindRect("_rect", &frame ) != 0 ) 
			{
				msg->SendRC( -1, "no _frame found." );
				return;
			}

			id = resizeWindow( msg, msg->source_pid(), id, &frame ); 
			break;
			
		case MOVE:
			if ( msg->FindInt("_id", &id ) != 0 ) 
			{
				msg->SendRC( -1, "no _id found." );
				return;
			}

			if ( msg->FindInt("_x", &x ) != 0 ) 
			{
				msg->SendRC( -1, "no _x found." );
				return;
			}
			
			if ( msg->FindInt("_y", &y ) != 0 ) 
			{
				msg->SendRC( -1, "no _y found." );
				return;
			}

			id = moveWindow( msg, msg->source_pid(), id, x, y ); 
			break;

			
		default:
			Looper::MessageReceived( msg );
			break;
	}
}

void Desktop::PulseReceived( Pulse *pulse )
{
   uint32 d1,d2,d3,d4,d5,d6;
   d1 = (*pulse)[0];
   d2 = (*pulse)[1];
   d3 = (*pulse)[2];
   d4 = (*pulse)[3];
   d5 = (*pulse)[4];
   d6 = (*pulse)[5];
   Window *win;
 
		
	switch (d1)
	{
	 case SYNC:
			win = windowBy( pulse->source_pid, (int)d2 );
			if ( win == NULL ) break;

			hideCursor( prev_x, prev_y, prev_buttons );
			syncWindow( win,
		                (int)d3,(int)d4,
						(int)d5,(int)d6 );
			showCursor( prev_x, prev_y, prev_buttons );
		   break;

	
	 case HIDE:
	 case SHOW:
		   showWindow( pulse->source_pid, (int)d2, (d3 == 1 ) );
		   break;

	
	 default:
			Looper::PulseReceived( pulse );
			break;
	}
}


// ------------------------------------------------



void Desktop::PutPixel( int x, int y, uint32 col )
{
	if ( (x < 0) || (x >= m_width ) ) return;
	if ( (y < 0) || (y >= m_height) ) return;

	if ( m_buffering == true ) m_double_buffer[ y * m_width + x ] = col;
				 		  else m_lfb[ y * m_width + x ] = col;
}

uint32 Desktop::GetPixel( int x, int y )
{
	if ( (x < 0) || (x >= m_width ) ) return 0;
	if ( (y < 0) || (y >= m_height) ) return 0;

	return m_lfb[ y * m_width + x ];
}

bool Desktop::initScreen()
{
	void *ptr;
	uint32 type;
	
	if ( smk_get_lfb_info( (uint32*)&m_width, 
					   (uint32*)&m_height, 
					   (uint32*)&m_depth, 
					   &type, &ptr ) != 0 )
		return false;

	int pages = (m_width * m_height * sizeof( unsigned int ) ) / 4096 + 1;
	

	// First, the double buffer.
	m_double_buffer = (uint32*)malloc( pages * 4096 );
	if ( m_double_buffer == NULL ) return false;
	
	
	m_lfb = (uint32*)smk_mem_map( ptr, pages );
	if ( m_lfb == NULL ) 
	{
		free( m_double_buffer );
		return false;
	}


	return true;
}

}




