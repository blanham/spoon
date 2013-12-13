#include <spoon/ipc/Pulse.h>
#include <spoon/interface/MenuItem.h>
#include <spoon/interface/Screen.h>

#include "CommanderApp.h"
#include "CommanderWindow.h"


CommanderApp::CommanderApp()
: Application( "commander" )
{
	Rect frame = Screen::Frame();

		 frame.left  	+= 100;
		 frame.right 	-= 100;
		 frame.top 		+= 100;
		 frame.bottom 	-= 100;
	
	CommanderWindow *commanderWindow = new CommanderWindow( frame );
					 commanderWindow->Show();
					 commanderWindow->Refresh();
}


