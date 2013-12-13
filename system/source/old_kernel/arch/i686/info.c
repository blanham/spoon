#include <process.h>
#include <threads.h>
#include <dmesg.h>
#include <switching.h>

#include "io.h"

#include "thread_data.h"


#warning This should not be necessary in a correct and proven kernel.

#include <pulse.h>
#include <messages.h>

int spitter(struct process* p)
{
	struct thread *tr = p->threads;
	struct thread_data *data;
	uint32 *stack;

	int y = 2;

	dmesg_xy(0, y++, "****  (%i) %s %s %i\n",
							p->pid, 
							process_states[ p->state ],
							p->name,
							p->kernel_threads );

	  while (tr != NULL)
	  {
		if ( tr == current_thread() )
		{
			dmesg_xy( 0, y++, "SKIPPED %s (actively running)\n", tr->name );
			tr = tr->linear_next;
			continue;
		}

		data = (struct thread_data*)tr->data;

		stack = (uint32*)(data->stack);

		dmesg_xy( 0, y++, "%p (%i,%i) (%s)  %s\n", 
							stack[11], 
							pulses_waiting( tr ),
							messages_waiting( tr ),
							states[ tr->state ], tr->name );

		tr = tr->linear_next;
	  }
	
	dmesg_xy(0, y++, "%s\n","Press ESC...                     ");
	dmesg_xy(0, y++,        "                                 ");
	  
	while (inb(0x60) != 1 ) {}
	while (inb(0x60) == 1 ) {}
	  
	return 0;
}


