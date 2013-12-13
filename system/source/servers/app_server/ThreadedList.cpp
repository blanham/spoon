#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "ThreadedList.h"


int ps()
{
	struct system_information si;
	struct process_information pi;
	int32 pid;
	int32 next;

	smk_conio_clear();
	smk_info_system( &si );

	printf("%s\n",  "SYSTEM INFORMATION --------------------------");
	printf("%s%s\n","            kernel_name: ", si.kernel_name );
	printf("%s%i\n","              cpu speed: ", si.cpu_speed );
	printf("%s%i\n","              total mem: ", si.memory_total );
	printf("%s%i\n","               free mem: ", si.memory_free );
	printf("%s%i\n","processes in the system: ", si.process_length );
	printf("%s%i\n","  threads in the system: ", si.thread_length );
	printf("%s%i\n","      threads scheduled: ", si.sched_length );
	printf("%s%i\n","        threads running: ", si.sched_running );
	printf("%s\n",  "-------------- PROCESS LIST -----------------");


	pid = 1;

          printf("%4s.%12s%5s%5s%32s%10s\n", 
		    "pid", 
		    "mem",
		    "cpu",
		    "sec",
		    "name",
		    "state" 
		    );


	while ( pid > 0 )
	{
		next = smk_info_process( pid, &pi );

		if ( next >= 0 )
		{
		   printf("%4i.%12i%5i%5i%32s%10s\n", 
		   	    pid, 
			    pi.usage_umem,
			    pi.usage_cpu / 1000,
			    pi.usage_seconds,
			    pi.name,
			    process_state[ pi.state ]
			    );
		}

		pid = next;
	}

  return 0;
}


ThreadedList::ThreadedList( char *name )
: Thread( name )
{
}


int ThreadedList::Run()
{
   while ( Terminated() == false )
    {
       ps();
       smk_sleep(1);
    }

   return 0;
}


