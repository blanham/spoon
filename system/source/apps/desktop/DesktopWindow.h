#ifndef _DESKTOP_WINDOW_H
#define _DESKTOP_WINDOW_H

#include <spoon/interface/Rect.h>
#include <spoon/interface/Window.h>

class DesktopWindow : public Window
{
	public:
		DesktopWindow( Rect frame );

		void Focus( bool focus );


		void PulseReceived( Pulse *pulse );

};


#endif

