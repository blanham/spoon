#ifndef _TERMINAL_WINDOW_H
#define _TERMINAL_WINDOW_H

#include <types.h>
#include <kernel.h>
#include <stdlib.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Window.h>
#include <spoon/interface/ConsoleView.h>


class TerminalWindow : public Window
{
  public:
    TerminalWindow( Rect frame )
     : Window( frame, "Terminal" )
     {
       ConsoleView *console;
       SetBackgroundColour( 0 );
       
	   int size   = smk_getenv_size( "TERMINAL" );
	   void *data = malloc( size );

	   smk_getenv( "TERMINAL", data, size );
	   
		   console = new ConsoleView( Bounds(), "TERMINAL", data, size  );
	       AddChild( console );

	   free( data );
     }
     
};




#endif

