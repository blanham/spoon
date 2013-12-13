#ifndef _TERMINAL_APP_H
#define _TERMINAL_APP_H

#include <types.h>
#include <spoon/app/Application.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Window.h>
#include <spoon/interface/ConsoleView.h>


class TerminalWindow : public Window
{
  public:
    TerminalWindow( Rect frame, const char *file = NULL )
     : Window( frame, "Terminal" )
     {
       ConsoleView *console;
       SetBackgroundColour( 0 );
       

	   int size   = smk_getenv_size( "TERMINAL" );
	   if ( size > 0 )
	   {
	   		void *data = malloc( size );
	   		smk_getenv( "TERMINAL", data, size );
		   
			console = new ConsoleView( Bounds(), "TERMINAL", data, size  );

			free( data );
	   }
	   else
	   {
	   
			if ( file == NULL )
	      			console = new ConsoleView( Bounds(), "/spoon/system/bin/shell" );
          	else
	      			console = new ConsoleView( Bounds(), file );

	   }
	   

	
		  
       AddChild( console );
     }
     
};




class TerminalApp : public Application
{
  public:
     TerminalApp( const char *file = NULL )
      : Application( "terminal" )
      {
        Rect frame( 50,50, (50 + 80 * 8), (50 + 25 * 16) );
      	(new TerminalWindow( frame, file ))->Show();
      }
};

#endif

