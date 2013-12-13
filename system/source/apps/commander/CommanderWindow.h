#ifndef _COMMANDER_WINDOW_H
#define _COMMANDER_WINDOW_H

#include <spoon/interface/Rect.h>
#include <spoon/interface/Window.h>
#include <spoon/interface/TextInput.h>
#include <spoon/interface/ListView.h>


#define PULSE_BACK		0x400
#define PULSE_UP		0x401
#define PULSE_REFRESH	0x402
#define PULSE_LOAD		0x403
#define PULSE_SELECT	0x404

class CommanderWindow : public Window
{
	public:
		CommanderWindow( Rect frame );


		bool Refresh();
		bool Select();
		bool Up();

		void PulseReceived( Pulse *pulse );


	private:
		bool cleanPath();


		bool isDirectory( const char *node );
		bool canHandle( const char *filename, char **command, char **params );

		
		
		TextInput *input;
		ListView *list;

};


#endif

