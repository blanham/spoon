#include <spoon/interface/View.h>
#include <spoon/interface/Button.h>
#include <spoon/interface/Window.h>


Button::Button( const char *label, 
	     Message *msg,
             Rect frame, 
	     uint32 follows )
: View( frame, follows ),  _msg( msg ), _pulse(NULL), _pressed( false )
{
  _label = label;
}

Button::Button( const char *label, 
	     Pulse *pulse,
             Rect frame, 
	     uint32 follows )
: View( frame, follows ), _msg( NULL ), _pulse(pulse), _pressed( false )
{
  _label = label;
}


Button::~Button()
{
	if ( _msg != NULL ) delete _msg;
	if ( _pulse != NULL ) delete _pulse;
}

/* ---------------------------------------------------- */


void Button::MouseDown( int x, int y, unsigned int buttons )
{
    SetPressed( true );
    Draw( Bounds() );
	Sync();
}

void Button::MouseUp( int x, int y, unsigned int buttons )
{
    SetPressed( false );
    Draw( Bounds() );
	Sync();
}

bool Button::SetPressed( bool press )
{
  if ( (_pressed == true) && (press == false) )
  {
	   if ( GetWindow() != NULL )
	   {
		 /// \todo This really should be PostMessage and PostPulse. Examine.
	     if ( _msg != NULL )
	       GetWindow()->PostMessage( _msg );
	
	     if ( _pulse != NULL )
	       GetWindow()->PostPulse( _pulse );
	   }
  }

  _pressed = press;

  return press;
}


/* ---------------------------------------------------- */

void Button::Draw( Rect frame )
{
  int y = Bounds().Height() / 2 - 8;
  int x = Bounds().Width() / 2;
      
      x = x - (Label()->Length() / 2 * 8);

	FillRect( 0,0, Bounds().right, Bounds().bottom,
		   BackgroundColour() );

   if ( Pressed() )
	DrawRect( 0,0, Bounds().right, Bounds().bottom, 0xFFFFFF );
   else
	DrawRect( 0,0, Bounds().right, Bounds().bottom, 0 );
      
  DrawString(  x, y, (char*) Label()->GetString() , 0 );
}





