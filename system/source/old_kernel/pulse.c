#include <configure.h>
#include <atomic.h>
#include <alloc.h>
#include <threads.h>
#include <scheduler.h>
#include <ports.h>
#include <conio.h>
#include <def_pulse.h>
#include <smk.h>



int32 send_pulse( int source_pid, 
		     int source_port,
		     struct process *proc, 
		     int dest_port, 
		     uint32 data1,
		     uint32 data2,
		     uint32 data3,
		     uint32 data4,
		     uint32 data5,
		     uint32 data6 )
{
	struct pulse *p;
	struct pulse *tmp;
	struct thread *tr;
	
	uint32 position;
	uint32 max;

	int tid;

	tid = port2tid( proc, dest_port );		// Find target tid
	if ( tid < 0 ) return -1;

	tr = find_thread_with_id( proc, tid );		// Find target port
	if ( tr == NULL ) return -1;


	// Find the last pulse page we have allocated.
	
	p = proc->ipc[dest_port].last_pulses;

	if ( p == NULL )
	{
		p = malloc( PULSE_PAGES * PAGE_SIZE );
		p[0].source_pid = 0;   // pointer to next page
		p[0].data4 = 1; // first taken pulse space in this page.
		p[0].data1 = 0; // Number of pulses waiting.
		proc->ipc[dest_port].pulses = p;
	}

	// Find the maximum. So we can have 0.. max - 1 . but 0 is the header
	max = (PULSE_PAGES * PAGE_SIZE) / sizeof( struct pulse );

	// But is this page full yet?
	//     Starting at 0 and going to (max - 1) => max is 1 beyond page
	if ( (p[0].data1 + p[0].data4) == max )
	{
		tmp = malloc( PULSE_PAGES * PAGE_SIZE );
		p[0].source_pid = (uint32)tmp;
		tmp[0].source_pid = 0;
		tmp[0].data4 = 1;
		tmp[0].data1 = 0;
		p = tmp;
	}

	proc->ipc[dest_port].last_pulses = p;	// Keep last page in sync

	// We have a nice, safe pulse page now in p
	// And we get it's position from the header pulse
	
		position = p[0].data4 + p[0].data1 ;  // First position;
	
		p[position].source_pid   = source_pid;
		p[position].source_port  = source_port;
		p[position].dest_port    = dest_port;
		p[position].data1 = data1;
		p[position].data2 = data2;
		p[position].data3 = data3;
		p[position].data4 = data4;
		p[position].data5 = data5;
		p[position].data6 = data6;
	
		p[0].data1 = p[0].data1 + 1;
	

		
		// WAKE UP THE THREAD IF IT WANTS TO BE WOKEN UP
	   	if ( port_flags( proc, dest_port, IPC_MASKED ) == 0 )
		{
		   if ( tr->state == THREAD_DORMANT )
		   {
		     tr->sleep_seconds = 0;
		     set_thread_state( tr, THREAD_RUNNING );
		   }
		}

	   
	return 0;
}


int32 receive_pulse( struct thread *tr,
		     int *source_pid,
		     int *source_port,
		     int *dest_port,
		     uint32 *data1,
		     uint32 *data2,
		     uint32 *data3,
		     uint32 *data4,
		     uint32 *data5,
		     uint32 *data6 )
{
	struct process *proc;
	struct pulse *p;
	uint32 position;
	uint32 max;
	int port;
	int i;
	

	proc = tr->process;

	// --------- FIRST HONOUR THE LAST PULSE HINT
	
	port = tr->last_port_pulse;
	tr->last_port_pulse = -1;

	if ( (port >= 0) && (port < MAX_PORTS) )
	{
				
        if ( port_flags( proc, port, IPC_MASKED ) != 0 ) return -1;
	  	p = proc->ipc[port].pulses;
        if ( p == NULL ) return -1;
		if ( p[0].data1 == 0 ) return -1;
		
		max = (PULSE_PAGES * PAGE_SIZE) / sizeof( struct pulse );
			  
		position = p[0].data4;
		
		*source_pid  = p[position].source_pid;
		*source_port = p[position].source_port;
		*dest_port   = p[position].dest_port;
	
		*data1 = p[position].data1;
		*data2 = p[position].data2;
		*data3 = p[position].data3;
		*data4 = p[position].data4;
		*data5 = p[position].data5;
		*data6 = p[position].data6;
		
		atomic_dec( & p[0].data1 );  // We have one less pulse waiting.
		atomic_inc( & p[0].data4 );  // We move ahead one pulse.
		
	          // Okay, is this whole page is clean?
		if ( p[0].data4 == max )
		{
		     // free this page and move along.
		  proc->ipc[port].pulses = (void*)( p[0].source_pid );
		  free( p );
		  if ( proc->ipc[port].pulses == NULL ) 
				  proc->ipc[port].last_pulses = NULL;
			// Keep last_pulses in sync
		}
		else
		 {
		    if ( p[0].data1 == 0 ) p[0].data4 = 1;
		 }
	
		return 0;			  
	  }
	

	// --------------------------- NOW DO THE REST OF THEM	
	
	for ( i = 0; i < MAX_PORTS; i++ )
	{
		if ( tr->ports[i] == -1 ) break;

		// -------------------------------------------
		port = tr->ports[i];
		if ( port_flags( proc, port, IPC_MASKED ) != 0 ) continue;
	  	p = proc->ipc[port].pulses;
		
	    	if ( p == NULL ) continue;
		if ( p[0].data1 == 0 ) continue;
	
		max = (PULSE_PAGES * PAGE_SIZE) / sizeof( struct pulse );
		  
		position = p[0].data4;
	
		*source_pid  = p[position].source_pid;
		*source_port = p[position].source_port;
		*dest_port   = p[position].dest_port;
	
		*data1 = p[position].data1;
		*data2 = p[position].data2;
		*data3 = p[position].data3;
		*data4 = p[position].data4;
		*data5 = p[position].data5;
		*data6 = p[position].data6;
	
		atomic_dec( & p[0].data1 );  // We have one less pulse waiting.
		atomic_inc( & p[0].data4 );  // We move ahead one pulse.
	
	          // Okay, is this whole page is clean?
		if ( p[0].data4 == max )
		{
	     // free this page and move along.
		  proc->ipc[port].pulses = (void*)( p[0].source_pid );
		  free( p );
		  if ( proc->ipc[port].pulses == NULL ) 
				  proc->ipc[port].last_pulses = NULL;
		  // Keep last_pulses in sync
		}
		else
		 {
		    if ( p[0].data1 == 0 ) p[0].data4 = 1;
		 }
		
		return 0;
	}

	return -1;
}



int32 preview_pulse( struct thread *tr,
		     int *source_pid, 
		     int *source_port,
		     int *dest_port,
		     uint32 *data1,
		     uint32 *data2,
		     uint32 *data3,
		     uint32 *data4,
		     uint32 *data5,
		     uint32 *data6 )
{
	struct process *proc;
	struct pulse *p;
	uint32 position;
	uint32 max;
	int port;
	int i;


	proc = tr->process;
	tr->last_port_pulse = -1;
	
	for ( i = 0; i < MAX_PORTS; i++ )
	{
	    if ( tr->ports[i] == -1 ) break;

	    port = tr->ports[i];
	    if ( port_flags( proc, port, IPC_MASKED ) != 0 ) continue;
	    p = proc->ipc[port].pulses;
	    if ( p == NULL ) continue;
	    if ( p[0].data1 == 0 ) continue;
	
		// -------------------------------------------
	    max = (PULSE_PAGES * PAGE_SIZE) / sizeof( struct pulse );
		  
	    position = p[0].data4;
	
		*source_pid  = p[position].source_pid;
		*source_port = p[position].source_port;
		*dest_port   = p[position].dest_port;
	
		*data1 = p[position].data1;
		*data2 = p[position].data2;
		*data3 = p[position].data3;
		*data4 = p[position].data4;
		*data5 = p[position].data5;
		*data6 = p[position].data6;
	
		tr->last_port_pulse = port;	// hint for drop/receive
		return 0;
	}

	return -1;
}



int32 pulses_waiting( struct thread *tr )
{
	int32 count;
	struct pulse *tmp;
	struct process *proc;
	int i;

	count = 0;

	proc = tr->process;
	
	for ( i = 0; i <  MAX_PORTS; i++ )
	{
	  if ( tr->ports[i] == -1 ) break;
	  if ( port_flags( proc, tr->ports[i], IPC_MASKED ) != 0 ) continue;
	  
	    tmp = proc->ipc[ tr->ports[i] ].pulses;
	    while (tmp != NULL )
	    {
		 count += tmp[0].data1;
		 tmp = (void*)(tmp[0].source_pid);
	    }
    }

	return count;
}


int32 drop_pulse( struct thread *tr )
{
	struct process *proc;
	struct pulse *p;
	uint32 max;
	int port;
	int i;

	proc = tr->process;
	

	// --------- FIRST HONOUR THE LAST PULSE HINT
	
	port = tr->last_port_pulse;
	tr->last_port_pulse = -1;

	if ( (port >= 0) && (port < MAX_PORTS) )
	  {
		      
	      if ( port_flags( proc, port, IPC_MASKED ) != 0 ) return -1;
	      if ( proc->ipc[port].pulses == NULL )  return -1;
			 
  		p = proc->ipc[port].pulses;

		if ( p[0].data1 == 0 ) return -1;
		
		max = (PULSE_PAGES * PAGE_SIZE) / sizeof( struct pulse );
		
		atomic_dec( & p[0].data1 );  // We have one less pulse waiting.
		atomic_inc( & p[0].data4 );  // We move ahead one pulse.
		
		// Okay, is this whole page is clean?
		if ( p[0].data4 == max )
		{
	          // free this page and move along.
		  proc->ipc[port].pulses = (void*)( p[0].source_pid );
		  free( p );
		  if ( proc->ipc[port].pulses == NULL ) 
			  proc->ipc[port].last_pulses = NULL;
	 	  // Keep last_pulses in sync
		}
		else
		 {
		    if ( p[0].data1 == 0 ) p[0].data4 = 1;
		 }
			  
		return 0;			  
	}
	

	// --- NOW DO THE REST OF THEM IF NOTHING IS MARKED
	
	for ( i = 0; i < MAX_PORTS; i++ )
	{
		if ( tr->ports[i] == -1 ) break;

		// -------------------------------------------
		
		port = tr->ports[i];

	  	p = proc->ipc[port].pulses;
	        if ( p == NULL ) continue;
		if ( p[0].data1 == 0 ) continue;
	
		max = (PULSE_PAGES * PAGE_SIZE) / sizeof( struct pulse );
		  
		atomic_dec( & p[0].data1 );  // We have one less pulse waiting.
		atomic_inc( & p[0].data4 );  // We move ahead one pulse.
	
	          // Okay, is this whole page is clean?
		if ( p[0].data4 == max )
		{
		     // free this page and move along.
		  proc->ipc[port].pulses = (void*)( p[0].source_pid );
		  free( p );
		  if ( proc->ipc[port].pulses == NULL ) 
			  proc->ipc[port].last_pulses = NULL;
		    // Keep last_pulses in sync
		}
		else
		 {
		    if ( p[0].data1 == 0 ) p[0].data4 = 1;
		 }
		  
		return 0;
	}


	return -1;
}


