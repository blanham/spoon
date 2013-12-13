#ifndef _SPOON_INTERFACE_BUTTON_H
#define _SPOON_INTERFACE_BUTTON_H


#include <spoon/support/String.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Rect.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Pulse.h>


/** \ingroup interface 
 *
 */
class Button : public View
{
   public:
     Button( const char *label,
     	     Message *msg,
             Rect frame, 
	     uint32 follows = FOLLOW_ALL_SIDES );

     Button( const char *label,
     	     Pulse *pulse,
             Rect frame, 
	     uint32 follows = FOLLOW_ALL_SIDES );


     ~Button();

	virtual void Draw( Rect frame );

	bool Pressed() { return _pressed; }
	bool SetPressed( bool press = true );

	String *Label() { return &_label; }


	virtual void MouseDown( int x, int y, unsigned int buttons );
	virtual void MouseUp( int x, int y, unsigned int buttons );

   private:
     String _label;
     Message *_msg;
     Pulse *_pulse;
     bool _pressed;
};


#endif

