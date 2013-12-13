#include <stdlib.h>
#include <spoon/interface/InterfaceDefs.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Screen.h>

#include <spoon/app/System.h>


#include "DesktopWindow.h"
#include "DesktopView.h"
#include "DesktopBar.h"

#include "TerminalWindow.h"

#include "globals.h"


DesktopWindow::DesktopWindow( Rect frame )
: Window( frame, "desktop",  WINDOW_HAS_NOFRAME | WINDOW_DESKTOP  )
{
	SetBackgroundColour( 0x304060 );
	SetPulseRate( 4000 );

	Rect barFrame = Bounds();

	AddChild( new DesktopView( barFrame ) );
	
		 barFrame.bottom = DESKBAR_HEIGHT;

	AddChild( new DesktopBar( barFrame ) );
}


void DesktopWindow::Focus( bool focus )
{
	if ( focus == false ) return;

	if ( systemMenuWindow->IsHidden() == false )
		systemMenuWindow->Hide();
}

void DesktopWindow::PulseReceived( Pulse *pulse )
{
	Rect frame;
		
	switch ((*pulse)[0])
	{
		case 0x600:
			if ( System::Execute( "/spoon/system/bin/commander", "", false ) 
							>= 0 ) break;
			break;
	
			
		case 0x602:
			if ( System::Execute( "/spoon/system/bin/terminal", "", false ) 
							>= 0 ) break;

			frame = Rect( 50,50, (50 + 80 * 8), (50 + 25 * 16) );

	      	(new TerminalWindow( frame ))->Show();
			break;
			
		case 0x603:
			smk_reboot();
			break;
			
		case 0x666:
			systemMenuWindow->Show();
			break;
			
		default:
			Window::PulseReceived( pulse );
			break;
	}
}




