#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <spoon/interface/Rect.h>
#include <spoon/interface/PopupMenu.h>
#include <spoon/interface/MenuItem.h>


// *****************************************************************


MenuItem::MenuItem( const char *str, Pulse *pulse )
: View( Rect(0,0,1,1), FOLLOW_LEFT_RIGHT )
{
	m_selected = false;
	m_str = strdup(str);
	m_message = NULL;
	m_pulse = NULL;
	if ( pulse != NULL ) m_pulse = new Pulse(*pulse);
}

MenuItem::MenuItem( const char *str, Message *message )
: View( Rect(0,0,1,1) )
{
	m_selected = false;
	m_str = strdup(str);
	m_pulse = NULL;
	m_message = NULL;
	if ( message != NULL ) m_message = new Message(*message);
}

MenuItem::~MenuItem()
{
	if ( m_message != NULL ) delete m_message;
	if ( m_pulse != NULL ) delete m_pulse;
	if ( m_str != NULL ) free( m_str );
}

// *****************************************************************

void MenuItem::Draw( Rect frame )
{
	if ( IsSelected() )
	{
		FillRect( Bounds(), 0x505050 );
		DrawString( 1, 1, m_str, 0xFFFFFF );
	}
	else
	{
		DrawString( 1, 1, m_str, 0 );
	}
}
		
// *****************************************************************

void MenuItem::MouseUp( int x, int y, unsigned int buttons )
{
	if ( GetWindow() == NULL ) return;

	PopupMenu *window = (PopupMenu*)GetWindow();
	
	window->Trigger( this, m_pulse, m_message );
}


// *****************************************************************

bool MenuItem::IsSelected()
{
	return m_selected;
}

void MenuItem::Select( bool select )
{
	m_selected = select;
	Draw( Bounds() );

	// Draw the menu item.
	Rect frame = Frame();
	Sync( 0, 0, frame.right, frame.bottom );
}


