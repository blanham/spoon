#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "../flist.h"
#include "ps.h"

struct function_info info_ps = 
{
   "process information",
   "ps [pid [tid]]\n"
   "This function will list all processes in the system. The"
   " optional \"pid\" (process id) parameter will allow you to"
   " list the threads owned by a particular process. The"
   " optional \"tid\" (thread id) combined with a \"pid\" will"
   " print information about a particular thread in a process.",
   ps_main
};


int ps_all( )
{
	struct system_information si;
	struct process_information pi;
	int32 pid;
	int32 next;

	smk_info_system( &si );

	printf("%s\n",  "SYSTEM INFORMATION --------------------------");
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


int ps_pid( int pid )
{
	struct process_information pi;
	struct thread_information ti;
	int32 next;	
	int32 tid;	
	
	printf("%s%i\n","Current pid: ",pid );
		
	if ( smk_info_process( pid, &pi ) < 0 )
	{
	  printf("%s\n","Unable to get process information");
	  return -1;
	}

	printf("%s%s\n","     name: ", pi.name );
	printf("%s%i\n","      pid: ", pi.pid );
	printf("%s%i\n","    state: ", pi.state );
	printf("%s","-----------------------------------------\n");

	next = 0;
	while ( next >= 0 )
	{
	  tid = next;
	  next = smk_info_thread( pid, next, &ti );
	  if ( next >= 0 )
	  {
	  	printf("%i. %s  %s  %p:%p\n", 
			tid, 
			ti.name, 
			thread_state[ ti.state ],
			ti.tss.cs,
			ti.tss.eip
			);
	  }
	  
	  if ( next == 0 ) break;
	}

	return 0;
}

int ps_tid( int pid, int tid )
{
	struct thread_information ti;
	printf("%s%i/%i\n","Current pid/tid: ",pid,tid );
	printf("%s\n","-------------------------------" );
	  
	if ( smk_info_thread( pid, tid, &ti ) < 0 )
	{
		printf("%s\n","unable to get thread information");
		return -1;
	}

  printf("%s%p %s%p %s%p %s%p\n",
		  "EAX = ",ti.tss.eax,
		  "EBX = ",ti.tss.ebx,
		  "ECX = ",ti.tss.ecx,
		  "EDX = ",ti.tss.edx
	 );

  printf("%s%p %s%p %s%p %s%p\n",
		  "GS = ",ti.tss.gs,
		  "ES = ",ti.tss.es,
		  "FS = ",ti.tss.fs,
		  "DS = ",ti.tss.ds
	 );

  printf("%s%p\n","EFLAGS = ",ti.tss.eflags);
  

  printf("%s%p:%p\n","CS:EIP = ",ti.tss.cs,ti.tss.eip);
  printf("%s%p:%p\n","SS:ESP = ",ti.tss.ss,ti.tss.esp);

  return 0;
}


int ps_main( int argc, char **argv )
{
   switch (argc)
   {
     case 1:  return ps_all(); 
     case 2:  return ps_pid( atoi( argv[1] ) );
     case 3:  return ps_tid( atoi( argv[1] ), atoi( argv[2] ) );
     default: return -1;
   };

   return 0;
}


