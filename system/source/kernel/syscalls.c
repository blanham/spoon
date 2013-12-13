#include <atomic.h>
#include <assert.h>
#include <inttypes.h>
#include <interrupts.h>
#include <process.h>
#include <syscalls.h>

#include <irq.h>

#include <scheduler.h>
#include <eflags.h>

#include <ioperms.h>

#include <ualloc.h>
#include <env.h>
#include <pci.h>

//DONE.DONE.
int syscall_memory( uint32_t r6, uint32_t r5, uint32_t r4, 
					uint32_t r3, uint32_t r2, uint32_t r1)
{
      int num;
      uint32_t *data;
      uint32_t loc;
      struct process *proc;


      switch( r1 & 0xFF )
      {
      	  // alloc/free
		  case SYS_ONE:
					if ( r2 == 0 )
					{
			    		proc = checkout_process( current_pid(), WRITER );
  		  				assert( proc != NULL );
						data = (void*)user_alloc( proc, r3 );
						commit_process( proc );
						return (int)data;
					}
					else if ( r2 == 1 )
					{
			    		proc = checkout_process( current_pid(), WRITER );
  		  				assert( proc != NULL );
				    	user_free( proc, r3 );
						commit_process( proc );
						return 0;
					}

	  	        return -1;

	case SYS_TWO:
			proc = checkout_process( current_pid(), WRITER );
  		  	assert( proc != NULL );
			
				loc = user_map( proc, r2, r3 );

			commit_process( proc );
			return loc;
			

	// returns size of area allocated
	case SYS_THREE:
			proc = checkout_process( current_pid(), READER );
  		  	assert( proc != NULL );
			
		    	num = user_size( proc, r2 );

			commit_process( proc );
		    return num;
		   
		
	// returns the physical location of the block
	case SYS_FOUR:
			proc = checkout_process( current_pid(), READER );
  		  	assert( proc != NULL );
		    
				loc = user_location( proc, r2 );
			
		    commit_process( proc );
		    return loc;
		 

      }


    return -1;
}




//DONE.DONE.
int syscall_process(uint32_t r6, uint32_t r5, uint32_t r4, 
					uint32_t r3, uint32_t r2, uint32_t r1) 
{
  struct process *proc;
  //void *pointer;
  int ans;
  
  switch( r1 & 0xFF )
  {
	  case SYS_ONE:
		  *((int32_t*)r2) = current_pid();
		  *((int32_t*)r3) = current_tid();
		  return 0;

/*
		  
	  case SYS_TWO:
		  proc = checkout_process( r2, WRITER );
		  if ( proc == NULL ) return -1;
		  
		  	set_process_state( proc, PROCESS_RUNNING );
			set_thread_state( proc->threads, THREAD_RUNNING );
		
		  commit_process( proc );
		  return 0;

	  case SYS_THREE:
		  proc = checkout_process( current_pid(), WRITER );
  		  assert( proc != NULL );

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
			assert( proc != NULL );

				pointer = proc->tls_location;
				if ( r2 == 0 ) *((void**)(r3)) = pointer; // safe to crash
				if ( r2 == 1 ) proc->tls_location = (void**)r3;

			commit_process( proc );
			return 0;
*/

	case SYS_FIVE:
			ans = -1;

			// Checkout appropriately.
			if ( (r2 == 0) || (r2 == 4) )	
				 proc = checkout_process( current_pid(), WRITER );
			else proc = checkout_process( current_pid(), READER );
			
			assert( proc != NULL );


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
int syscall_thread( uint32_t r6, uint32_t r5, uint32_t r4, 
					uint32_t r3, uint32_t r2, uint32_t r1)
{
  uint32_t *data;
  int answer;
  struct thread *tr;
  struct process *proc;
  //void *pointer;

  switch ( r1 & 0xFF )
  {
	  case SYS_ONE:
			proc = checkout_process( current_pid(), WRITER );
			assert( proc != NULL );

			    data = (uint32_t*)r4;
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
			assert( proc != NULL );

			answer = -1;
			
			    tr = get_thread( proc, r2 );
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

/*
			
	  case SYS_THREE:
			proc = checkout_process( current_pid(), READER );
			assert( proc != NULL );
			
				answer = -1;
		    	tr = find_thread_with_name( proc, (char*)r3);
				if ( tr != NULL ) answer = tr->tid;
				
			commit_process( proc );
		    return answer;


	  case SYS_FOUR:
			return go_dormant( r2 );
		

	  case SYS_FIVE:
			proc = checkout_process( current_pid(), WRITER );
			assert( proc != NULL );

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
		assert( proc != NULL );

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


		*/
  }

  return -1;
}



//DONE.DONE.
int syscall_pci( uint32_t r6, uint32_t r5, uint32_t r4, 
				 uint32_t r3, uint32_t r2, uint32_t r1 )
{
  int ans;
  
  switch ( r1 & 0xFF )
  {
    case SYS_ONE:
			ans = pci_find_cfg( r2, r3, r4, ( struct pci_cfg* )r5 );
      		return ans; 

	case SYS_TWO:
			ans = pci_find_probe( r2, r3, r4, (struct pci_cfg*) r5 );
			return ans;

  };

  return -1;
}


//DONE.DONE.
int syscall_irq( uint32_t r6, uint32_t r5, uint32_t r4, 
				 uint32_t r3, uint32_t r2, uint32_t r1)
{
   struct process *proc;
   struct thread *tr;
   int answer = -1;

    switch( r1 & 0xFF )
    {
       case SYS_ONE:
			  proc = checkout_process( current_pid(), WRITER );
		  	  assert( proc != NULL );
			  
				  tr = get_thread( proc, r4 );
				  if ( tr != NULL )
				  {
					  answer = request_irq( tr, r3 );
				  }

			  commit_process( proc );
			  return answer;
			  
	    case SYS_TWO:
		  proc = checkout_process( current_pid(), WRITER );
		  assert( proc != NULL );

		  	answer = release_irq( current_thread(),  r3 );

		  commit_process( proc );
		  return answer;
		  
		case SYS_THREE:
		  proc = checkout_process( current_pid(), WRITER );
		  assert( proc != NULL );

		  	dmesg("%!IRQ_ACK syscall\n");
		  
			  answer = irq_ack( current_thread(), (int)r2, (int)r3 );

		  commit_process( proc );
		  
		  disable_interrupts();
			current_thread()->sched_state = THREAD_IRQ;
		  enable_interrupts();

		  sched_yield();
		  return answer;
    };

  return -1;
}


//DONE.DONE.
int syscall_io( uint32_t r6, uint32_t r5, uint32_t r4, 
				uint32_t r3, uint32_t r2, uint32_t r1 )
{
  int ans = -1;
  struct process *proc = NULL;


  proc = checkout_process( current_pid(), WRITER );
  assert( proc != NULL );

  dmesg("%!Process %s required IOPRIV\n", proc->name );

  switch( r1 & 0xFF )
  {
	  case SYS_ONE:
			 if ( r2 == 0 )  ans = io_grant_priv( proc );
			 if ( r2 == 1 )  ans = io_revoke_priv( proc );
			break;
				 
  }

  commit_process( proc );
  return ans;
}



int syscall( uint32_t r6, uint32_t r5, uint32_t r4, 
			 uint32_t r3, uint32_t r2, uint32_t r1)
{
  uint8_t system_call;
  uint8_t function;
  //struct thread *tr;
  int rc;

  system_call = (r1 >> 8) & 0xFF;
     function = (r1 & 0xFF);

   // capability check
	if ( system_call < 1 ) return -1;
	if ( system_call > TOTAL_SYSCALLS ) return -1;
	if ( function == 0 ) return -1;

/*
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
	
	

*/

	atomic_inc( & (current_process()->kernel_threads) );
	
	rc = -1;
	  
	switch (system_call)
	{
     case SYS_MEMORY:	 rc = syscall_memory( r6, r5, r4, r3, r2, r1);	break;

/*
     case SYS_SEMAPHORE: rc = syscall_semaphore(r6,r5, r4, r3, r2, r1); break;
     case SYS_SYSTEM:	 rc = syscall_system( r6, r5, r4, r3, r2, r1);	break;
*/
     case SYS_PROCESS:	 rc = syscall_process( r6, r5, r4, r3, r2, r1);	break;
     case SYS_THREAD:	 rc = syscall_thread( r6, r5, r4, r3, r2, r1);	break;

/*
     case SYS_CAP:	 	 rc = syscall_cap( r6, r5, r4, r3, r2, r1);		break;
     case SYS_PULSE: 	 rc = syscall_pulse( r6, r5, r4, r3, r2, r1);	break;
     case SYS_MESSAGE: 	 rc = syscall_message( r6, r5, r4, r3, r2, r1);	break;
     case SYS_PORT: 	 rc = syscall_port( r6, r5, r4, r3, r2, r1);	break;
     case SYS_EXEC: 	 rc = syscall_exec( r6, r5, r4, r3, r2, r1);	break;
*/
						 
     case SYS_PCI: 	 	 rc = syscall_pci( r6, r5, r4, r3, r2, r1);		break;
/*
     case SYS_TIME: 	 rc = syscall_time( r6, r5, r4, r3, r2, r1);	break;
*/
     case SYS_IRQ: 	 	 rc = syscall_irq( r6, r5, r4, r3, r2, r1);		break;
     case SYS_IO: 	 	 rc = syscall_io( r6, r5, r4, r3, r2, r1);		break;
/*
     case SYS_CONSOLE: 	 rc = syscall_console( r6, r5, r4, r3, r2, r1);	break;
     case SYS_LFB:	 	 rc = syscall_lfb( r6, r5, r4, r3, r2, r1);		break;
     case SYS_INFO:      rc = syscall_info( r6, r5, r4, r3, r2, r1);	break;
     case SYS_REBOOT:    rc = syscall_reboot( r6, r5, r4, r3, r2, r1);	break;
     case SYS_SHMEM:     rc = syscall_shmem( r6, r5, r4, r3, r2, r1);	break;
*/
	}
  

	atomic_dec( & (current_process()->kernel_threads) );
  
  return rc;
}



/** Little stub function which retrieves information from the stack
 * and passes it up to the real syscall() function.
 *
 * Enables interrupts
 *
 */
int syscall_prepare( uint32_t edi, uint32_t esi, uint32_t ebp,
					 uint32_t temp, uint32_t ebx, uint32_t edx,
					 uint32_t ecx, uint32_t eax )
{
	uint32_t *stack = (uint32_t*)ecx;
		
	// sysenter disabled interrupts so we restore them
	enable_interrupts();
	
	stack[0] = syscall( stack[1], stack[2], stack[3], 
						stack[4], stack[5], stack[6] );
	return 0;
}



