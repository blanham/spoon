#include "SystemButton.h"


SystemButton::SystemButton( const char *label,
				     	     Message *msg,
				             Rect frame, 
						     uint32 follows)
: Button( label, msg, frame, follows )
{
}

SystemButton::SystemButton( const char *label,
				     	     Pulse *pulse,
				             Rect frame, 
						     uint32 follows )
: Button( label, pulse, frame, follows )
{
}



void SystemButton::Draw( Rect frame )
{
  int y = Bounds().Height() / 2 - 8;
  int x = Bounds().Width() / 2;
      
      x = x - (Label()->Length() / 2 * 8);

  uint32 col = 0;
  if ( Pressed() ) col = 0x60;

  DrawString(  x, y, (char*) Label()->GetString(), col );
}

