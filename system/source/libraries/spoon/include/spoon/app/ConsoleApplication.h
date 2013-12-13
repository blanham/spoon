#ifndef _SPOON_APP_CONSOLEAPPLICATION_H
#define _SPOON_APP_CONSOLEAPPLICATION_H

#include <types.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>
#include <spoon/app/Application.h>

/** \ingroup app
 *
 *
 */

class ConsoleApplication : public Application
{
   public:
	ConsoleApplication(const char *signature);
	~ConsoleApplication();
		
	void PulseReceived( Pulse *p ); 

	// Basic console facilities..
	int getch();
	int gets( char * );
	virtual int main( int argc, char *argv[] ) = 0;

	int Run();

	char *history(int i);
	void add_history( char* msg );
	
   private:
	int32 kbd_sem;
	int32 main_tid;
		
	char kbd_buffer[256];
	int kbd_position;
	int kbd_count;
	
	
	char **history_list;
	int history_count;
	int history_position;
	
};


#endif


