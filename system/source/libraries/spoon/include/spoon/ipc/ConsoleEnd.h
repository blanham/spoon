#ifndef _SPOON_IPC_CONSOLEEND_H
#define _SPOON_IPC_CONSOLEEND_H

#include <types.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Looper.h>

class Console;

/** \ingroup ipc 
 *
 *
 */
class ConsoleEnd : public Looper
{
  public:
    ConsoleEnd( int pages = 3, Console *console = NULL );
   ~ConsoleEnd();

     int id() { return _console_id; }

	virtual void PulseReceived( Pulse *pulse );



  private:
    int _console_id;
    Console *_console;
};

#endif

