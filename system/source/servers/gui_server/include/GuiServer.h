#ifndef _GUI_SERVER_H
#define _GUI_SERVER_H

#include <types.h>
#include <spoon/app/Application.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/interface/InterfaceDefs.h>

#include <Desktop.h>


class GuiServer : public Application
{
   public: 
     GuiServer();
     ~GuiServer();

     void MessageReceived( Message *msg );
     void PulseReceived( Pulse *p );


   private:

	 // ------

		int register_window( Message *msg );
		int register_direct_window( Message *msg );
		int register_video_driver( Message *msg );
		int registration_message( Message *msg );
	 

		int get_message( Message *msg );
	 
	 GUI::Desktop* desktop;
	 
	 
	 // ------

};

#endif


