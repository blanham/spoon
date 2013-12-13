#include <types.h>
#include <spoon/app/Application.h>
#include <spoon/base/IRQHandler.h>

#ifndef _KBDDRIVER_H
#define _KBDDRIVER_H


class	KBDDriver : public Application, public IRQHandler
{
	public:
		KBDDriver( char* proc_name = NULL );
		~KBDDriver();
		int IRQ( int32 irq );
		int Init();
		void Shutdown();




		int SendMessage(  uint32 what, uint32 unicode, uint32 scancode, uint32 modifiers );

		
	private:
		int server_pid;

		unsigned int leds;
		unsigned int modifiers;
		unsigned int extended;


};

#endif

