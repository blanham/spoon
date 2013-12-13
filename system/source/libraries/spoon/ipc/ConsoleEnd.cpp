#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Looper.h>
#include <spoon/ipc/ConsoleEnd.h>
#include <spoon/base/Console.h>

ConsoleEnd::ConsoleEnd( int pages, Console *console )
: Looper(), _console( console )
{
   int p = pages;
   if ( p < 3 ) p = 3;

   _console_id = smk_create_console( p, VC_MODE_MESSAGE, smk_getpid(), Port(), -1 );
}

ConsoleEnd::~ConsoleEnd()
{
  smk_delete_console( _console_id );
}



void ConsoleEnd::PulseReceived( Pulse *pulse )
{
  switch ((*pulse)[0])
  {
     case VC_CLEAR:
       if ( _console != NULL )
         _console->Clear( ((*pulse)[1] & 0xF0) >> 4, ((*pulse)[1] & 0xF) );
       break;
     case VC_GOTOXY:
       if ( _console != NULL )
         _console->GotoXY( (*pulse)[1],  (*pulse)[2] );
       break;
     case VC_PUTC:
       if ( _console != NULL )
         _console->PutChar( (*pulse)[1], 
	 		   ((*pulse)[2] & 0xF0) >> 4, 
			   ((*pulse)[2] * 0xF) ); 
       break;
     default:
       Looper::PulseReceived( pulse );
       break;
  }
}


