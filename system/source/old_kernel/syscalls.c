#include <types.h>
#include <configure.h>
#include <macros.h>
#include <atomic.h>
#include <conio.h>
#include <syscalls.h>
#include <switching.h>
#include <threads.h>
#include <memory.h>
#include <process.h>
#include <scheduler.h>
#include <waits.h>
#include <ports.h>
#include <pulse.h>
#include <paging.h>
#include <lfb.h>
#include <sem.h>
#include <messages.h>
#include <shmem.h>
#include <ualloc.h>
#include <console.h>
#include <pci.h>
#include <irq.h>
#include <ioperms.h>
#include <time.h>
#include <strings.h>
#include <exec.h>
#include <info.h>
#include <interrupts.h>
#include <misc.h>
#include <cpu.h>
#include <random.h>
#include <env.h>


#warning data verification.


// -------------------------------------------------------------------------------

//DONE.DONE.
int syscall_memory( uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
      int num;
      uint32 *data;
      uint32 loc;
      struct process *proc;


      switch( r1 & 0xFF )
      {
      	  // alloc/free
		  case SYS_ONE:
					if ( r2 == 0 )
					{
			    		proc = checkout_process( current_pid(), WRITER );
  		  				ASSERT( proc != NULL );
						data = (void*)user_alloc( proc, r3 );
						commit_process( proc );
						return (int32)data;
					}
					else if ( r2 == 1 )
					{
			    		proc = checkout_process( current_pid(), WRITER );
  		  				ASSERT( proc != NULL );
				    	user_free( proc, r3 );
						commit_process( proc );
						return 0;
					}

	  	        return -1;

	case SYS_TWO:
			proc = checkout_process( current_pid(), WRITER );
  		  	ASSERT( proc != NULL );
			
				loc = user_map( proc, r2, r3 );

			commit_process( proc );
			return loc;
			

	// returns size of area allocated
	case SYS_THREE:
			proc = checkout_process( current_pid(), READER );
  		  	ASSERT( proc != NULL );
			
		    	num = user_size( proc, r2 );

			commit_process( proc );
		    return num;
		   
		
	// returns the physical location of the block
	case SYS_FOUR:
			proc = checkout_process( current_pid(), READER );
  		  	ASSERT( proc != NULL );
		    
				loc = user_location( proc, r2 );
			
		    commit_process( proc );
		    return loc;
		 

      }
      return -1;
}



//DONE.DONE.
int syscall_semaphore(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
   int answer = -1;
   
   switch ( r1 & 0xFF )
   {
      case SYS_ONE:  	
		if ( r2 == 0 ) 
				 answer = create_local_semaphore( current_process(), r3 );
		 	else answer = create_global_semaphore( current_pid(), r3 );
		break;
		
      case SYS_TWO: 	
		if ( r2 == 0 )
				 answer = destroy_local_semaphore( current_process(), r3 );
			else answer = destroy_global_semaphore( current_pid(), r3 );
		break;

      case SYS_THREE: 	
		if ( r2 == 0 )
				 answer = wait_local_semaphore( current_thread(), r3 ); 
			else answer = wait_global_semaphore( current_thread(), r3 );
		break;
						
      case SYS_FOUR:
		if ( r2 == 0 )
				 answer = signal_local_semaphore( current_thread(), r3 );
			else answer = signal_global_semaphore( current_thread(), r3 );
		break;
		
   }

  return answer;
}


//DONE.DONE.
int syscall_system(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
	struct process *proc;
	int answer;

	switch ( r1 & 0xFF )
	{
 	   case SYS_ONE:
			return find_process( (char*)r3 );

	   case SYS_TWO:
		   proc = checkout_process( r2, READER );
		   if ( proc == NULL ) return -1;
		   
		       memcpy( (char*)r3, proc->name, OS_NAME_LENGTH );
		       answer = proc->pid;

		   commit_process( proc );
	       return answer;

       
	   case SYS_THREE:
		   proc = checkout_process( r2, READER );
		   if ( proc == NULL ) return -1;
		   
		       answer = proc->pid;

		   commit_process( proc );
	       return answer;
	};

	
	return -1;
}




//DONE.DONE.
int syscall_process(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1) 
{
  struct process *proc;
  void *pointer;
  int ans;
  
  switch( r1 & 0xFF )
  {
	  case SYS_ONE:
		  *((int32*)r2) = current_pid();
		  *((int32*)r3) = current_tid();
		  return 0;
		  
	  case SYS_TWO:
		  proc = checkout_process( r2, WRITER );
		  if ( proc == NULL ) return -1;
		  
		  	set_process_state( proc, PROCESS_RUNNING );
			set_thread_state( proc->threads, THREAD_RUNNING );
		
		  commit_process( proc );
		  return 0;

	  case SYS_THREE:
		  proc = checkout_process( current_pid(), WRITER );
  		  ASSERT( proc != NULL );

		  	proc->rc = (int)r2;

			disable_interrupts();
			  atomic_dec( & (proc->kernel_threads) );
			  set_process_state( proc, PROCESS_FINISHING );
			  set_thread_state( current_thread(), THREAD_FINISHED );
		  	  commit_process( proc );
			enable_interrupts();
		  
		  while (1==1) {sched_yield(); };
		  
      case SYS_FOUR:
		    if ( ((r2 != 0) && (r2 != 1)) ) return -1;
			
			proc = checkout_process( current_pid(), WRITER );
			ASSERT( proc != NULL );

				pointer = proc->tls_location;
				if ( r2 == 0 ) *((void**)(r3)) = pointer; // safe to crash
				if ( r2 == 1 ) proc->tls_location = (void**)r3;

			commit_process( proc );
			return 0;


	case SYS_FIVE:
			ans = -1;

			// Checkout appropriately.
			if ( (r2 == 0) || (r2 == 4) )	
				 proc = checkout_process( current_pid(), WRITER );
			else proc = checkout_process( current_pid(), READER );
			
			ASSERT( proc != NULL );


			if ( r2 == 0 )
			{
				ans = set_environment( proc, (char*)r3, (void*)r4, (int)r5 );
			}
			else if ( r2 == 1 )
			{
				ans = get_environment( proc, (char*)r3, (void*)r4, (int)r5 );
			}
			else if ( r2 == 2 )
			{
				ans = get_environment_size( proc, (char*)r3, (int*)r4 );
			}
			else if ( r2 == 3 )
			{
				ans = get_environment_information( proc, (int)r3,
												  (char*)r4,
												  (int*)r5 );
			}
			else if ( r2 == 4 )
			{
				ans = remove_environment( proc, (char*)r3 );
			}

			commit_process( proc );
			return ans;
			

			
			
  }
  return -1;
}


//DONE.DONE.
int syscall_thread(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
  uint32 *data;
  int answer;
  struct thread *tr;
  struct process *proc;
  void *pointer;

  switch ( r1 & 0xFF )
  {
	  case SYS_ONE:
			proc = checkout_process( current_pid(), WRITER );
			ASSERT( proc != NULL );

			    data = (uint32*)r4;
			    answer = new_thread( 0,
					  		   proc, 
							     current_thread(),
							     (const char*)r3,
							     r6,
							     r2,
							     data[0],
							     data[1],
							     data[2],
							     data[3]
							   );
	
			commit_process( proc );
		    return answer;
				   
	  case SYS_TWO:
			proc = checkout_process( current_pid(), WRITER );
			ASSERT( proc != NULL );

			answer = -1;
			
			    tr = find_thread_with_id( proc, r2 );
			    if ( tr != NULL ) 
			    {
					disable_interrupts();
			    	set_thread_state( tr, r3 );
					enable_interrupts();
					if ( r2 == current_tid() ) sched_yield();
					answer = 0;
			    }

			commit_process( proc );
		    return answer;

	  case SYS_THREE:
			proc = checkout_process( current_pid(), READER );
			ASSERT( proc != NULL );
			
				answer = -1;
		    	tr = find_thread_with_name( proc, (char*)r3);
				if ( tr != NULL ) answer = tr->tid;
				
			commit_process( proc );
		    return answer;


	  case SYS_FOUR:
			return go_dormant( r2 );
		

	  case SYS_FIVE:
			proc = checkout_process( current_pid(), WRITER );
			ASSERT( proc != NULL );

			    tr = current_thread();
			    tr->rc = (int)r2;
				
				disable_interrupts();
			  		atomic_dec( & (proc->kernel_threads) );
				    set_thread_state( tr, THREAD_FINISHED );
					commit_process( proc );
				enable_interrupts();
			   	sched_yield();	// Never to return.
				
		    return 0;
			
	case SYS_SIX:
		proc = checkout_process( current_pid(), WRITER );
		ASSERT( proc != NULL );

			if ( ((int)r3) == -1 )  tr = current_thread(); // save time.
			else
			   tr = find_thread_with_id( current_process(), (int)r3 );
	
		        if ( ((r2 != 0) && (r2 != 1)) || (tr==NULL)  ) 
				{
					commit_process( proc );
				   	return -1;
		        }
	
			pointer = tr->tls;
			if ( r2 == 1 ) tr->tls = (void*)r4;
			if ( r2 == 0 ) *((void**)(r4)) = pointer; // safe to crash
	
		commit_process( proc );
		return 0;
  }

  return -1;
}



//DONE.DONE.
int syscall_cap(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
  struct thread *tr;
  struct thread *target;
  struct process *proc;
  uint8 system_call;
  uint8 function;
  int answer;


	// r2 - pid,  r3 - tid,  r4 - call
	
	    system_call = (( r4 >> 0x8 ) & 0xFF);
	       function = ( r4 & 0xFF );
	
	 	if ( system_call < 1 ) return -1;
		if ( system_call > TOTAL_SYSCALLS ) return -1;
	
  
	// ----- CHECK FOR VALIDITY -------------------------
  	proc = checkout_process( current_pid(), READER );
	ASSERT( proc != NULL );
  
      tr = current_thread();
	
			  // local and remote set
	  if ( ((r1 & 0xFF) == SYS_ONE) || ((r1 & 0xFF) == SYS_THREE) ) 
	  {
	 	// can't grant/revoke capabilities it doesn't have
		if ( (tr->capabilities[ system_call - 1 ] & function) != function )
		{
			commit_process( proc );
			return -1;
		}
	  }
	 
   commit_process( proc );
   // ------ WE HAVE VALIDITY ---------------------------
	
   switch (r1 & 0xFF)
   {
     case SYS_ONE:
		proc = checkout_process( r2, WRITER );
		if ( proc == NULL ) return -1;
			
			target = find_thread_with_id( proc, r3 );
			
			answer = -1;
	  	    if ( target != NULL ) 
		    {
		  		target->capabilities[ system_call - 1 ] = function;
				answer = 0;
		    }

		commit_process( proc );
   		return answer;
		   
		   
     case SYS_TWO:
			proc = checkout_process( r2, READER );
			if ( proc == NULL ) return 0;
			
				target = find_thread_with_id( proc, r3 );
				
				function = 0;
		  	    if ( target != NULL ) 
			    {
			  		function = target->capabilities[ system_call - 1 ];
			    }

			commit_process( proc );
   			return function;

     case SYS_THREE:
			proc = checkout_process( current_pid(), WRITER );
			ASSERT( proc != NULL );

			    target = find_thread_with_id( proc, r2 );
	
				answer = -1;
		  	    if ( target != NULL ) 
				{
				  target->capabilities[ system_call - 1 ] = function;
				  answer = 0;
				}

			commit_process( proc );
	   		return answer;
		   
		   
     case SYS_FOUR:
			proc = checkout_process( current_pid(), READER );
			ASSERT( proc != NULL );

			    target = find_thread_with_id( proc, r2 );

				function = 0;
	    	    if ( target != NULL ) 
			    {
				  function = target->capabilities[ system_call - 1 ] & 0xFF;
				}
		  
			commit_process( proc );
	   		return function;
   }

	return -1;
}



//DONE.DONE.
int syscall_pulse(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
  int32 answer;
  uint32 *data;
  struct process *proc;
  struct process *target;

  switch ( r1 & 0xFF )
  {
	  case SYS_ONE:
		  target = checkout_process( r3, WRITER );
		  if ( target == NULL ) return -1;
		  
			  data = (uint32*)r5;
			  answer = send_pulse( current_pid(),
			  			r2, 
			  			target, 
						r4,
						data[0],
						data[1], 
						data[2], 
						data[3], 
						data[4], 
						data[5] );

		  commit_process( target );
		  return answer;

	  case SYS_TWO:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );

			  data = (uint32*)r5;
			  answer = receive_pulse(  current_thread(),
			  			   (int32*) r2, 
			   			   (int32*) r3,
						   (int32*) r4,
			  			   & ( data[0] ), 
						   & ( data[1] ), 
						   & ( data[2] ), 
						   & ( data[3] ), 
						   & ( data[4] ), 
						   & ( data[5] ) );
		  
		  commit_process( proc );
		  return answer;
				  
	  case SYS_THREE:
		  proc = checkout_process( current_pid(), READER );
		  ASSERT( proc != NULL );
		  
			  data = (uint32*)r5;
			  answer = preview_pulse(  current_thread(),
			  			   (int32*) r2, 
			  			   (int32*) r3,
			  			   (int32*) r4,
			  			   & ( data[0] ), 
						   & ( data[1] ), 
						   & ( data[2] ), 
						   & ( data[3] ), 
						   & ( data[4] ), 
						   & ( data[5] ) );

		  commit_process( proc );
		  return answer;
				  
	  case SYS_FOUR: 
		  proc = checkout_process( current_pid(), READER );
		  ASSERT( proc != NULL );

		  	answer = pulses_waiting( current_thread() );
			
		  commit_process( proc );
		  return answer;

	  case SYS_FIVE:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );
		  
			  answer = drop_pulse( current_thread() );
		  
		  commit_process( proc );
		  return answer;
  }

  
	return -1;
}


//DONE.DONE.
int syscall_message( uint32 r6, uint32 r5, uint32 r4, 
		       uint32 r3, uint32 r2, uint32 r1 )
{
  struct process *proc;
  struct process *target;
  int32 answer;

   answer = -1;

  switch ( r1 & 0xFF )
  {
	  case SYS_ONE:
		  target = checkout_process( r3, WRITER );
		  if ( target == NULL ) return -1;
		  
			  answer = send_message( current_pid(),
						  			 r2,
						  			 target,
									 r4,
									 r5,
									 (void*)r6 );

		  commit_process( target );
		  return answer;

		// recv
	  case SYS_TWO:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );
		  
				  answer = recv_message( current_thread(),
							  			 (int32*)r2,
										 (int32*)r3,
										 (int32*)r4,
							  			 r5,
										 (void*)r6 );
  	
		  commit_process( proc );
		  return answer;
		
		// peek
	  case SYS_THREE:
		  proc = checkout_process( current_pid(), READER );
		  ASSERT( proc != NULL );
		  
				  answer = peek_message( current_thread(),
							  			 (int32*)r2,
							  			 (int32*)r3,
										 (int32*)r4,
										 (int32*)r5 );

		  commit_process( proc );
		  return answer;
		  
		// drop
	  case SYS_FOUR:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );
		  
			  answer = drop_message( current_thread() );
		  
		  commit_process( proc );
	  	  return answer;
				  
  }
  
  return answer;
}


//DONE.DONE.
int syscall_port( uint32 r6, uint32 r5, uint32 r4, 
		 		  uint32 r3, uint32 r2, uint32 r1 )
{
  struct process *proc;
  int answer = -1;


  switch ( r1 & 0xFF )
  {
	  case SYS_ONE:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );

			  answer = create_port( current_thread(), (int)r2, (int)r3 );

		  commit_process( proc );
		  return answer;

	  case SYS_TWO:
		  proc = checkout_process( current_pid(), READER );
		  ASSERT( proc != NULL );

			  answer = port2tid( proc, (int)r2 ); 

		  commit_process( proc );
		  return answer;
		
	  case SYS_THREE:
		  proc = checkout_process( current_pid(), READER );
		  ASSERT( proc != NULL );

			  answer = tid2port( proc, (int)r2 ); 

		  commit_process( proc );
		  return answer;
		  
	  case SYS_FOUR:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );
		  
		  	answer = release_port( current_thread(), (int)r2 ); 

		  commit_process( proc );
	  	  return answer;
	
	  case SYS_FIVE:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );

			  if ( r3 == 0 ) 
				   answer = set_port_flags( current_thread(), r2, 0 );
			     else 
				   answer = set_port_flags( current_thread(), r2, IPC_MASKED ); 

		  commit_process( proc );
		  return answer;
		  
	  case SYS_SIX:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );

			  if ( r2 == 0 )
					  answer = bulk_get_ports( current_thread(), (void*)r3 );
			  else if ( r2 == 1 )
					  answer = bulk_set_ports( current_thread(), (void*)r3 );
			
		  commit_process( proc );
		  return answer;
				  
  }
  
  return -1;
}


void godlike( struct process *proc );

//DONE.DONE.
int syscall_exec(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
  struct process *proc;
  int ans;

  switch ( r1 & 0xFF )
  {
	  case SYS_ONE:
		  if ( r6 == 0 ) return 0;
		  if ( r3 == 0 ) return 0;
		  ans = exec_memory_region( current_pid(), (char*)r3,
					      r2, r4, (char*)r6 );

		  if ( ans == -1 ) return -1;
		  
		  proc = checkout_process( ans, WRITER );
		  if ( proc == NULL ) return -1;  /// \todo freak;
		  
			  godlike( proc );

		  commit_process( proc );
		  return ans;
  }
		  
  return -1;	
}


//DONE.DONE.
int syscall_pci( uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1 )
{
  int ans;
  
  switch ( r1 & 0xFF )
  {
    case SYS_ONE:
			disable_interrupts();
			ans = pci_find_cfg( r2, r3, ( struct pci_cfg* )r4 );
			enable_interrupts();
      		return ans; 

	case SYS_TWO:
			disable_interrupts();
			ans = pci_find_probe( r2, r3, r4, (struct pci_cfg*) r5 );
			enable_interrupts();
			return ans;

  };

  return -1;
}


//DONE.
int syscall_time(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
   struct process *proc;
   time_t one,two;
   int answer;
   int* rc;

	 switch ( r1 & 0xFF )
	 {
		  case SYS_ONE: 
			  return epoch_seconds();
		  case SYS_TWO:
			  disable_interrupts();
			  system_time( &one, &two );
			  		// Increment the system time for this.
	    	  increment_system_time( current_process() );
			  enable_interrupts();
			  
			  *((uint32*)r2) = one;
			  *((uint32*)r3) = two;
			  return 0;

		  case SYS_THREE:
			  delay( r2 );
			  return 0;
			  
		  case SYS_FOUR:
			  system_time( &one, &two );
			  proc = checkout_process( current_pid(), WRITER );
		  	  ASSERT( proc != NULL );

			    current_thread()->sleep_seconds = one + r2;
			    current_thread()->sleep_milliseconds = two;
				
				disable_interrupts();
				    set_thread_state( current_thread(), THREAD_SLEEPING );
					commit_process( proc );
					atomic_dec( &(proc->kernel_threads) );
				enable_interrupts();
			    sched_yield();
				atomic_inc( &(proc->kernel_threads) );
			  return 0;

	      case SYS_FIVE:

			   rc = (int*)r4;
			  
			   if ( ((int)r3) >= 0 ) 
					 answer = begin_wait_thread( r2, r3, rc );
			    else answer = begin_wait_process( r2, rc );

			  return answer;

		  case SYS_SIX:
			  return sched_yield();

         } 


   return -1;
}

//DONE.DONE.
int syscall_irq(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
   struct process *proc;
   struct thread *tr;
   int answer = -1;

    switch( r1 & 0xFF )
    {
       case SYS_ONE:
			  proc = checkout_process( current_pid(), WRITER );
		  	  ASSERT( proc != NULL );
			  
				  tr = find_thread_with_id( proc, r4);
				  if ( tr != NULL )
				  {
					  answer = request_irq( tr, (char*)r2, r3 );
				  }

			  commit_process( proc );
			  return answer;
			  
	    case SYS_TWO:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );

		  	answer = release_irq( current_thread(),  r3 );

		  commit_process( proc );
		  return answer;
		  
		case SYS_THREE:
		  proc = checkout_process( current_pid(), WRITER );
		  ASSERT( proc != NULL );
		  
		  	  disable_interrupts();
			  	sched_ack( current_thread() );
			  	current_thread()->state = THREAD_IRQ;
			  	unmask_irq( r3 );
			  	commit_process( proc );
			  enable_interrupts();

		  sched_yield();
		  return 0;
    };

  return -1;
}



//DONE.DONE.
int syscall_io(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
  int ans = -1;
  struct process *proc = NULL;

  proc = checkout_process( current_pid(), WRITER );
  ASSERT( proc != NULL );

  switch( r1 & 0xFF )
  {
	  case SYS_ONE:
			 if ( r2 == 0 ) ans = io_grant( proc, r3 ); 
			 if ( r2 == 1 ) ans = io_revoke( proc, r3 );
			break;
			
			
	  case SYS_TWO:
			 if ( r2 == 0 )  ans = io_grant_priv( proc );
			 if ( r2 == 1 )  ans = io_revoke_priv( proc );
			break;
				 
  }

  commit_process( proc );
  return ans;
}



//DONE.DONE.
int syscall_console(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1) 
{
     struct process *proc;
     int ans;
     int old;
     uint32 p;

     switch ( r1 & 0xFF )
     {
       case SYS_ONE: 
			proc = checkout_process( current_pid(), READER ); 
  			ASSERT( proc != NULL );
			
				p = (uint32)proc->console_location;

			commit_process( proc );
       		return p;

       case SYS_TWO: 
			proc = checkout_process( current_pid(), READER ); 
  			ASSERT( proc != NULL );
			
				ans = console_size( proc->console );

			commit_process( proc );
       		return ans;
		
       case SYS_THREE:  
	       return set_active_console( r2 );

       case SYS_FOUR:  
			proc = checkout_process( r2, READER ); 
  			ASSERT( proc != NULL );

				ans = proc->console;
		   
			commit_process( proc );
	       	return ans;
	       
       case SYS_FIVE:
			proc = checkout_process( r2, WRITER );
  			ASSERT( proc != NULL );
			
			
			   old = proc->console;
			   detach_console( proc );
			   ans = attach_console( proc, r3 );
			   if ( ans != 0 )
				    attach_console( proc, old ); 

			commit_process( proc );
	       	return ans;


	case SYS_SIX:
		ans = -1;
			
			if ( r2 == 0 ) ans = new_console( r3 );
			if ( r2 == 1 ) ans = stage_console( (void*)r3, r4 );
			
		return ans;
		
	case SYS_SEVEN:
		return delete_console( r2 );

	case SYS_EIGHT:
		ans = -1;
		 if ( r2 == 0 ) ans = set_active_pid( r3, r4 );
		 if ( r2 == 1 ) ans = get_active_pid( r3 );
		return ans;

     }
	             
       
     return -1;
}


//DONE.
int syscall_lfb(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1) 
{
	int32 answer;
	struct process *proc;

	switch ( r1 & 0xFF )
	{
	  case SYS_ONE: 
			  answer = get_lfb( (uint32*)r2, (uint32*)r3, 
							  	(uint32*)r4, (uint32*)r5, ((void**)r6) );
			  return answer;

	  case SYS_TWO: return -1;
					
	  case SYS_THREE:
		  proc = checkout_process( current_pid(), WRITER );
  		  ASSERT( proc != NULL );
			
		  	answer = provide_lfb(  proc );

		  commit_process( proc );
		  return answer;
	};


	return -1;
}

//DONE.
int syscall_info(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1) 
{
   int32 answer = -1;
   struct process *proc		= NULL;
   struct thread *tr		= NULL;

	switch ( r1 & 0xFF )
	{
	     case SYS_ONE:
		     return info_system( (struct system_information*) r2 );

	     case SYS_TWO:
			 proc = checkout_process( r2, READER );
			 if ( proc == NULL ) return -1;
			 
			     answer = info_process( proc, (struct process_information*) r3 );

			 commit_process( proc );
		     return answer;
			 
	     case SYS_THREE:
			 proc = checkout_process( r2, READER );
			 if ( proc == NULL ) return -1;

			 tr = find_thread_with_id( proc, r3 );
			 if ( tr != NULL )
			 {
			     	answer = info_thread( tr, (struct thread_information*)r4 );
			 }
			 
			 commit_process( proc );
		     return answer;

	     case SYS_FOUR:
		     return random();
		     

		case SYS_FIVE:
			 return spit();

		case SYS_SIX:

			 if ( r2 == 0 ) return get_dmesg_size();

			 if ( r2 == 1 ) return get_dmesg( (void*)r3, (int)r4 );
			 
			 return -1;
	};

	return -1;
}

//DONE.DONE.
int syscall_reboot(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1) 
{

   switch ( r1 & 0xFF )
   {
     case SYS_ONE:
	      disable_interrupts();
		  while (1==1) arch_reboot();
	      return -1;
   }

   return -1;
}



//DONE.DONE.
int syscall_shmem(uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1) 
{
	struct process *proc;
	int ans = -1;

   switch ( r1 & 0xFF )
   {
     case SYS_ONE:
		return create_shmem( (char*)r2, current_pid(), (int)r3, (int)r4 );
		  
	 case SYS_TWO:
		  return delete_shmem( current_pid(), (int)r2 );

		  
		  
	 case SYS_THREE:
		  if ( r2 == 0 )
		  {
			return grant_shmem( current_pid(), (int)r3, (int)r4, (unsigned int)r5 );
		  }
		  if ( r2 == 1 )
		  {
			proc = checkout_process( (int)r4, WRITER );
			if ( proc == NULL ) return -1;

				ans = revoke_shmem( current_pid(), proc, (int)r3 );

			commit_process( proc );
			return ans;
		  }
		  return -1; 
		  
	 case SYS_FOUR:
		proc = checkout_process( current_pid(), WRITER );
		if ( proc == NULL ) return -1;


		  if ( r2 == 0 )
		  {
			ans = request_shmem( proc, (int)r3, 
								  (void**)r4,
								  (int*)r5,
								  (unsigned int*)r6 );
		  }
		  if ( r2 == 1 )
		  {
			ans = release_shmem( proc, (int)r3 );
		  }
			
		commit_process( proc );
		return ans;
		  
	 case SYS_FIVE:
			return find_shmem( (char*)r2, (int*)r3 );

	 case SYS_SIX:
			return get_shmem( (int)r2, (char*)r3, (int*)r4, (int*)r5, (unsigned int*)r6	);
	
   }

   
   return -1;
}




// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------



int syscall( uint32 r6, uint32 r5, uint32 r4, uint32 r3, uint32 r2, uint32 r1)
{
  uint8 system_call;
  uint8 function;
  struct thread *tr;
  int rc;

  system_call = (r1 >> 8) & 0xFF;
     function = (r1 & 0xFF);

   // capability check
	if ( system_call < 1 ) return -1;
	if ( system_call > TOTAL_SYSCALLS ) return -1;
	if ( function == 0 ) return -1;

	tr = current_thread();
	if ( (tr->capabilities[system_call - 1] & function) != function )
	{
	  return -1;
	}
   // authorised

	// Dying..... stop all threads from entering the kernel.
	
	if ( (current_process()->state == PROCESS_CRASHING) || 
		 (current_process()->state == PROCESS_FINISHING)    )
	{
		while (1==1) sched_yield();
	}
	
	

  atomic_inc( & (current_process()->kernel_threads) );
	
  rc = -1;
  
  switch (system_call)
  {
     case SYS_MEMORY:	 rc = syscall_memory( r6, r5, r4, r3, r2, r1);	break;
     case SYS_SEMAPHORE: rc = syscall_semaphore(r6,r5, r4, r3, r2, r1); break;
     case SYS_SYSTEM:	 rc = syscall_system( r6, r5, r4, r3, r2, r1);	break;
     case SYS_PROCESS:	 rc = syscall_process( r6, r5, r4, r3, r2, r1);	break;
     case SYS_THREAD:	 rc = syscall_thread( r6, r5, r4, r3, r2, r1);	break;
     case SYS_CAP:	 	 rc = syscall_cap( r6, r5, r4, r3, r2, r1);		break;
     case SYS_PULSE: 	 rc = syscall_pulse( r6, r5, r4, r3, r2, r1);	break;
     case SYS_MESSAGE: 	 rc = syscall_message( r6, r5, r4, r3, r2, r1);	break;
     case SYS_PORT: 	 rc = syscall_port( r6, r5, r4, r3, r2, r1);	break;
     case SYS_EXEC: 	 rc = syscall_exec( r6, r5, r4, r3, r2, r1);	break;
     case SYS_PCI: 	 	 rc = syscall_pci( r6, r5, r4, r3, r2, r1);		break;
     case SYS_TIME: 	 rc = syscall_time( r6, r5, r4, r3, r2, r1);	break;
     case SYS_IRQ: 	 	 rc = syscall_irq( r6, r5, r4, r3, r2, r1);		break;
     case SYS_IO: 	 	 rc = syscall_io( r6, r5, r4, r3, r2, r1);		break;
     case SYS_CONSOLE: 	 rc = syscall_console( r6, r5, r4, r3, r2, r1);	break;
     case SYS_LFB:	 	 rc = syscall_lfb( r6, r5, r4, r3, r2, r1);		break;
     case SYS_INFO:      rc = syscall_info( r6, r5, r4, r3, r2, r1);	break;
     case SYS_REBOOT:    rc = syscall_reboot( r6, r5, r4, r3, r2, r1);	break;
     case SYS_SHMEM:     rc = syscall_shmem( r6, r5, r4, r3, r2, r1);	break;
  }
  

	atomic_dec( & (current_process()->kernel_threads) );
  
  return rc;
}






