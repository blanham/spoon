#include <stdio.h>
#include <unistd.h>
#include <spoon/ipc/Looper.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Window.h>
#include <spoon/interface/PopupMenu.h>
#include <spoon/interface/MenuItem.h>

PopupMenu::PopupMenu( Looper *looper )
: Window( Rect(0,0,150,150), "system menu",  WINDOW_HAS_NOFRAME |
											 WINDOW_GETS_MOUSEMOVED )
{
	m_looper    = looper;
	m_itemCount = 0;
	m_lastItem  = NULL;
}


bool  PopupMenu::AddChild( View *view )
{
	Rect frame = Bounds();
	
		view->MoveTo( 0, m_itemCount * 19 );
		view->ResizeTo( Bounds().Width(), 19 );

	bool success = Window::AddChild( view );
	if ( success == true ) 
	{
		m_itemCount += 1;
		ResizeTo( Bounds().Width(), m_itemCount * 19 );
	}
	return success;
}


// ***************************************************

void PopupMenu::Draw( Rect frame )
{
	for ( int i = 0; i < Bounds().Height(); i++ )
	{
		uint32 col = 0xC0C0C0;
		if ( ((Frame().top + i) % 3) == 2 ) col = 0xB8B8C8;

		int width = Bounds().Width() - 1;

		DrawLine( 0, i, width, i, col );
	}

	DrawViews( frame );
	Sync();
}

// ***************************************************

void PopupMenu::Focus( bool focus )
{
	if ( focus == false ) Hide();
}


// ***************************************************

void PopupMenu::PulseReceived( Pulse *pulse )
{
	switch ((*pulse)[0])
	{
		default:
			Window::PulseReceived( pulse );
			break;
	}
}


void PopupMenu::MessageReceived( Message *msg )
{
	switch (msg->what)
	{
		default:
			Window::MessageReceived( msg );
			break;
	}
}


void PopupMenu::Trigger( MenuItem* item,
								  Pulse *pulse,
								  Message *message )
{
	Hide();
	if ( m_lastItem != NULL )
	{
		m_lastItem->Select( false );
		m_lastItem = NULL;
	}
	
	if ( m_looper == NULL ) return;
	if ( pulse != NULL ) m_looper->PostPulse( pulse );
	if ( message != NULL ) m_looper->PostMessage( message );
}



void PopupMenu::MouseEvent( int what, int x, int y, unsigned int buttons )
{
	MenuItem *child = (MenuItem*)ChildAt( x, y );
	if ( child == NULL ) return;	// no point to go on.

	if ( child != m_lastItem )
	{
		if ( m_lastItem != NULL )
		{
			m_lastItem->Select( false );
		}

		m_lastItem = child;
		
		if ( m_lastItem->IsSelected() == false )
		{
			m_lastItem->Select( true );
		}

		Draw( Bounds() );
	}
	
		
	// Let the Window do the real work.
	Window::MouseEvent( what, x, y, buttons );
}


