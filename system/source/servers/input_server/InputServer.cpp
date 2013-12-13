#include <types.h>
#include <kernel.h>
#include <spoon/sysmsg.h>
#include <stdio.h>
#include "InputServer.h"


// **********************************************

InputServer::InputServer()
: Application("input_server"), guiMode(false)
{
  uint32 tmp;
  void *ptr;

   if ( smk_get_lfb_info( &tmp, &tmp, &tmp, &tmp, &ptr ) == 0 )
     if ( ptr != 0 ) 
     {
		_gui_pid = smk_find_process_id( "gui_server" );
		if ( _gui_pid > 0 ) guiMode = true;
     }
}


// *********************************************


void InputServer::PulseReceived( Pulse *p )
{
   uint32 d1,d2,d3,d4,d5,d6;
   d1 = (*p)[0];
   d2 = (*p)[1];
   d3 = (*p)[2];
   d4 = (*p)[3];
   d5 = (*p)[4];
   d6 = (*p)[5];


   switch (d1)
   {
     case KEY_UP:
     case KEY_DOWN:
       if ( guiMode == false ) _gui_pid = smk_active_pid(0);

       if ( (d3 >= 59) && (d3 <= (69) ) )
		smk_set_console( d3 - 59 ); // F1 == 0
       else	
		Messenger::SendPulse( _gui_pid, 0, d1,d2,d3,d4,d5,d6 );

		break;

     case MOUSE_UP:
     case MOUSE_DOWN:
     case MOUSE_MOVED:
       if ( guiMode == false ) _gui_pid = smk_active_pid(0);
	   Messenger::SendPulse( _gui_pid, 0, d1,d2,d3,d4,d5,d6 );
       break;
     
     default:
       Application::PulseReceived( p );
       break;
   }
}


