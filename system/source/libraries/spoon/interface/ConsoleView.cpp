#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <spoon/sysmsg.h>
#include <spoon/base/Thread.h>
#include <spoon/base/Console.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/ipc/ConsoleEnd.h>
#include <spoon/interface/Window.h>
#include <spoon/interface/View.h>
#include <spoon/interface/ConsoleView.h>
#include <spoon/app/System.h>

// ----------------------------------------------------------

ConsoleView::ConsoleView( Rect frame, const char* filename )
: View( frame ),
  Console( 80, 25 ),
  _cursor_X(0),
  _cursor_Y(0)
{
   SetBackgroundColour( 0x0 );
   _cend = new ConsoleEnd( 3, this );
   _cend->Resume();	
   
   Clear( 7, 0);
   if ( filename != NULL ) Execute( filename );
}


ConsoleView::ConsoleView( Rect frame, const char *process_name,
							void *data, int size )
: View( frame ),
  Console( 80, 25 ),
  _cursor_X(0),
  _cursor_Y(0)
{
   SetBackgroundColour( 0x0 );
   _cend = new ConsoleEnd( 3, this );
   _cend->Resume();	
   
   Clear( 7, 0);
   Execute( process_name, data, size );
}



ConsoleView::ConsoleView( Rect frame, int pid )
: View( frame ),
  Console( 80, 25 ),
  _cursor_X(0),
  _cursor_Y(0)
{
   SetBackgroundColour( 0x0 );
   _cend = new ConsoleEnd( 3, this );
   _cend->Resume();

   Clear( 7, 0);
   if ( pid >= 0 ) Attach( pid );
}

ConsoleView::~ConsoleView()
{
  _cend->Terminate();
}

// ----------------------------------------------------------


int ConsoleView::Execute( const char* filename )
{
   int ans = System::Execute( filename, NULL, _cend->id(), false );
   if ( ans > 0 ) SetActivePid( ans );
   return ans;
}


int ConsoleView::Execute( const char *process_name, void *data, int size )
{
   int ans = System::Execute( process_name, data, size, 
				   			  NULL, _cend->id(), false );
   if ( ans > 0 ) SetActivePid( ans );
   return ans;
}


int ConsoleView::Attach( int pid )
{
   int ans = smk_switch_console( pid, _cend->id() );
   if ( ans == 0 )  smk_set_active_pid( _cend->id(), pid );
   return ans;
}


int ConsoleView::ActivePid()
{
   return smk_active_pid( _cend->id() );
}

int ConsoleView::SetActivePid( int pid )
{
   return smk_set_active_pid( _cend->id(), pid );
}


// ----------------------------------------------------------


void ConsoleView::PutChar( uint32 c, char fg, char bg )
{
	char message[2];

	  message[1] = 0;
	  message[0] = c;

	if ( (c == '\n') )
	{
		_cursor_X = 0;
		_cursor_Y = _cursor_Y + 1;
	}
	else
	if ( (c == '\b') )
	{
	   _cursor_X = _cursor_X - 1;
	   if ( _cursor_X > 80 )
	   {
	   	_cursor_X = 79;
		_cursor_Y--;
	   }

	   if ( _cursor_Y > 25 )
	   {
	   	_cursor_X = 0;
		_cursor_Y = 0;
	   }

	  FillRect( _cursor_X * 8, _cursor_Y * 16, 
		    	_cursor_X * 8 + 8, _cursor_Y * 16 + 16, 
		    	BackgroundColour() );

	  	Sync( _cursor_X * 8, _cursor_Y * 16,
			  _cursor_X * 8 + 8, _cursor_Y * 16 + 16 );
	  
	  _buffer[ _cursor_X + _cursor_Y * 80 ] = ' ';
	}
	else
	{
	  DrawString( _cursor_X * 8, _cursor_Y * 16, message, 0xFFFFFF );
	  
	  	Sync( _cursor_X * 8, _cursor_Y * 16,
			  _cursor_X * 8 + 8, _cursor_Y * 16 + 16 );
					  
	  _buffer[ _cursor_X + _cursor_Y * 80 ] = c;
	  _cursor_X = _cursor_X + 1;
	}

	if ( _cursor_X == 80 ) 
	{
		_cursor_X = 0;
		_cursor_Y = _cursor_Y + 1;
	}

	if ( _cursor_Y == 25 ) ScrollUp( fg, bg );

}


void ConsoleView::Clear( char fg, char bg )
{
  FillRect( Bounds(), 0 );
  for ( int i = 0; i < 80 * 25; i++ )
    _buffer[i] = ' ';
  GotoXY(0,0);
  Sync();
}

void ConsoleView::GotoXY( int x, int y )
{
 _cursor_X = x;
 _cursor_Y = y;
 if ( _cursor_X < 0 ) _cursor_X = 0;
 if ( _cursor_Y < 0 ) _cursor_Y = 0;
 if ( _cursor_X >= Cols() ) _cursor_X = Cols() - 1;
 if ( _cursor_Y >= Rows() ) _cursor_Y = Rows() - 1;
}


void ConsoleView::ScrollUp( char fg, char bg )
{
     for ( int i = 0; i < 80; i++ )
  	for ( int j = 0; j < 24; j++ )
	{
	    _buffer[j * 80 + i] = _buffer[j * 80 + i + 80];
	}


	for ( int i = 0; i < 80; i++ )
	  _buffer[ 24 * 80 + i ] = ' ';

	Draw( Bounds() );
	Sync();

  _cursor_Y -= 1;
 if ( _cursor_Y < 0 ) _cursor_Y = 0;
}




// ----------------------------------------------------------

void ConsoleView::Draw( Rect area )
{
  char message[2];
       message[1] = 0;

  for ( int i = 0; i < 80; i++ )
   for ( int j = 0; j < 25; j++ )
   {
     message[0] = _buffer[j * 80 + i]; 
     DrawString( i * 8, j * 16, message, 0xFFFFFF, BackgroundColour() );
   }
}


void ConsoleView::KeyDown( uint32 code, uint32 ascii, uint32 modifiers )
{
	Messenger::SendPulse( smk_active_pid( _cend->id() ), 0, KEY_DOWN, ascii, ascii, modifiers );
}




