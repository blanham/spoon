#include <stdio.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/interface/Screen.h>

#include <spoon/video/GUIControl.h>

Screen::Screen( )
{
}

Screen::~Screen()
{
}

Rect Screen::Frame( int _screen_id )
{
	GUIControl gui;

	int width;
	int height;
	int mode;
	int id;
	
	if ( gui.GetDesktopMode( _screen_id, &id, &width, &height, &mode ) != 0 )
		return Rect( 0,0,-1,-1);


    return Rect( 0,0, width - 1, height - 1);
}




