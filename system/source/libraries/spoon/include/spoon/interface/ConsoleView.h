#ifndef _SPOON_INTERFACE_CONSOLEVIEW_H
#define _SPOON_INTERFACE_CONSOLEVIEW_H

#include <types.h>
#include <spoon/ipc/Looper.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/interface/View.h>
#include <spoon/base/Console.h>

class ConsoleEnd;


/** \ingroup interface 
 *
 */
class ConsoleView : public View, public Console
{
   public:
     ConsoleView( Rect frame, const char* filename = NULL );
     ConsoleView( Rect frame, const char *process_name, void *data, int size );
     ConsoleView( Rect frame, int pid );
     virtual ~ConsoleView();


     int Execute( const char* filename );
     int Execute( const char* process_name, void *data, int size );
     int Attach( int pid );

     int ActivePid();
     int SetActivePid(int pid);

	// Console overrides
     virtual void PutChar( uint32 c, char fg, char bg );
     virtual void Clear( char fg, char bg );
     virtual void GotoXY( int x, int y );
     
     virtual void ScrollUp( char fg, char bg );
     
     virtual void Draw( Rect area );
     virtual void KeyDown( uint32 code, uint32 ascii, uint32 modifiers );

    private:
     ConsoleEnd *_cend;

     char _buffer[ 80 * 25 ];
     int _cursor_X;
     int _cursor_Y;
};

#endif


