#include <types.h>
#include <spoon/app/Application.h>
#include <spoon/base/IRQHandler.h>

#ifndef _PS2DRIVER_H
#define _PS2DRIVER_H



class	PS2Driver : public Application, public IRQHandler
{
	public:
		PS2Driver( int mode, char* proc_name = NULL );
		~PS2Driver();

		int IRQ_0( int irq );
		int IRQ_1( int irq );
		int IRQ_2( int irq );
		int IRQ_3( int irq );
		
		int IRQ( int irq );

		int Init();
		void Shutdown();

	private:
		int m_mode;
		int server_pid;
};

#endif

