#ifndef _SYSTEM_BUTTON_H
#define _SYSTEM_BUTTON_H

#include <spoon/interface/Button.h>

class SystemButton : public Button
{
	public:
			
		SystemButton( const char *label,
	     	     Message *msg,
	             Rect frame, 
			     uint32 follows = FOLLOW_ALL_SIDES );
	
		SystemButton( const char *label,
	     	     Pulse *pulse,
	             Rect frame, 
			     uint32 follows = FOLLOW_ALL_SIDES );
	

		virtual void Draw( Rect frame );
};


#endif

