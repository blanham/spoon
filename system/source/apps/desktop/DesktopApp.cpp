#include <spoon/ipc/Pulse.h>
#include <spoon/interface/MenuItem.h>

#include "DesktopApp.h"
#include "DesktopWindow.h"


#include "globals.h"

void additem( const char *str, unsigned int val )
{
	MenuItem *sam = new MenuItem( str, new Pulse( val ) );
	systemMenuWindow->AddChild( sam );
}

DesktopApp::DesktopApp()
: Application( "desktop" )
{
	// Create the desktop window (aka Desktop)
	Rect frame = Screen::Frame();
	desktopWindow = new DesktopWindow( frame );
	desktopWindow->Show();
	
	// Create the desktop system menu
	frame.Set( 0, DESKBAR_HEIGHT + 1, 150, DESKBAR_HEIGHT + 401 );
	systemMenuWindow = new PopupMenu( desktopWindow ); 

	systemMenuWindow->MoveTo( 0, DESKBAR_HEIGHT + 1 );
	systemMenuWindow->ResizeTo( 150, DESKBAR_HEIGHT + 401 );

	
	additem( "---", 0 );
	additem( "Commander", 0x600 );
	additem( "---", 0 );
	additem( "Terminal", 0x602 );
	additem( "---", 0 );
	additem( "Reboot", 0x603 );
}


