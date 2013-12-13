#include <smk.h>
#include <alloc.h>
#include <strings.h>
#include <process.h>
#include <threads.h>
#include <messages.h>
#include <ports.h>
#include <ipc.h>
#include <conio.h>


#include <def_message.h>

// Currently, the whole kernel needs to be locked
// for these functions to be safe...



int send_message( int source_pid,
				  int source_port,
				  struct process* proc,
				  int dest_port,
				  int bytes,
				  void *location )
{
	struct message *msg;
	struct thread *tr;
	int dest_tid;
	int size;

	
	dest_tid = port2tid( proc, dest_port );
	if ( dest_tid < 0 ) return -1;

	tr = find_thread_with_id( proc, dest_tid );
	if ( tr == NULL ) return -1;
	
	size = (bytes + sizeof(struct message));

	msg 				= malloc( size );
	msg->source_pid   	= source_pid;
	msg->source_port  	= source_port;
	msg->dest_port    	= dest_port;
	msg->bytes 			= bytes;
	msg->next  			= NULL;
	msg->data  			= (void*)((uint32)msg + sizeof( struct message ));
				// data points to the next byte


	memcpy( msg->data, location, bytes );

	if ( proc->ipc[dest_port].messages == NULL )
	{					// Nothing in msg queue
	  proc->ipc[dest_port].messages = msg;
	}
	else
	{					// Previous messages
	  proc->ipc[dest_port].last_message->next = msg;		// Add to last
	}

	proc->ipc[dest_port].last_message = msg;			// Update last msg.

	// WAKE UP THE THREAD, maybe

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

int recv_message( struct thread* tr,
		  int *source_pid,
		  int *source_port,
		  int *dest_port,
		  int bytes,
		  void *location )
{
	struct message *tmp;
	int size;
	int pages;
	int i;
	int port;
	struct process *proc = tr->process;


	// --------- First obey the hint supplied in thread structure
	
	port = tr->last_port_message;
	tr->last_port_message = -1;

	
	if ( (port >= 0) && (port < MAX_PORTS) )
	{

	   if ( port_flags( proc, port, IPC_MASKED ) != 0 ) return -1;
	   if ( proc->ipc[port].messages == NULL ) return -1;
		 
		// ----- message to return
		
		tmp = proc->ipc[port].messages;			// Remove the first message
		proc->ipc[port].messages = tmp->next;
			
		if ( proc->ipc[port].messages == NULL ) 
			proc->ipc[port].last_message = NULL;
								// Keep the last message
								// correct.
		
		size = tmp->bytes;
		if ( bytes < size ) size = bytes;
		
		*source_pid  = tmp->source_pid;
		*source_port = tmp->source_port;
		*dest_port   = tmp->dest_port;
		

		/*
		*/

		
		memcpy( location, tmp->data, size );
		pages = (tmp->bytes + sizeof(struct message)) / PAGE_SIZE + 1;
		free( tmp );
		return size;
   }


	// ------ NOW SEARCH EVERYTHING -------------------------
	
	for ( i = 0; i < MAX_PORTS; i++ )
	{
		if ( tr->ports[i] == -1 ) break;

		port = tr->ports[i];

		if ( port_flags( proc, port, IPC_MASKED ) != 0 ) continue;
		if ( proc->ipc[port].messages == NULL ) continue;

		// ----- message to return
		
		tmp = proc->ipc[port].messages;			// Remove the first message
		proc->ipc[port].messages = tmp->next;
			
		if ( proc->ipc[port].messages == NULL ) 
			proc->ipc[port].last_message = NULL;
								// Keep the last message
								// correct.
		
		size = tmp->bytes;
		if ( bytes < size ) size = bytes;
		
		*source_pid  = tmp->source_pid;
		*source_port = tmp->source_port;
		*dest_port   = tmp->dest_port;
		
		memcpy( location, tmp->data, size );
		
		pages = (tmp->bytes + sizeof(struct message)) / PAGE_SIZE + 1;
		free( tmp );
		return size;
	}
		
	return -1;
}


int peek_message( struct thread* tr,
		  int *source_pid,
		  int *source_port,
		  int *dest_port,
		  int *bytes )
{

	struct message *tmp;
	int i;
	int port;
	struct process *proc = tr->process;


	tr->last_port_message = -1;

	for ( i = 0; i < MAX_PORTS; i++ )
	{
		if ( tr->ports[i] == -1 ) break;

		port = tr->ports[i];

		if ( port_flags( proc, port, IPC_MASKED ) != 0 ) continue;
		if ( proc->ipc[port].messages == NULL ) continue;

		tmp = proc->ipc[port].messages;

		// ----- message to return

		tr->last_port_message = port;
		
		*source_pid  = tmp->source_pid;
		*source_port = tmp->source_port;
		*dest_port   = tmp->dest_port;
		*bytes	     = tmp->bytes;
	
		return 0;
	}
		
	return -1;
}



int messages_waiting( struct thread *tr )
{
	struct message *tmp;
	int count;
	int port;
	int i;
	struct process *proc = tr->process;

	count = 0;
	
	for ( i = 0; i < MAX_PORTS; i++ )
	{
		if ( tr->ports[i] == -1 ) break;

		port = tr->ports[i];

		if ( port_flags( proc, port, IPC_MASKED ) != 0 ) continue;

		// ----- message to return
	
			/// \todo speed this up by maintaining a count?
		tmp = proc->ipc[port].messages;		
		while ( tmp != NULL )
		{
			tmp = tmp->next;
			count++;
		}
			
	}
		
	return count;
}



int drop_message( struct thread *tr )
{
	struct message *tmp;
	int port;
	int pages;
	int i;
	struct process *proc = tr->process;


	// ----- First honour the suggested port in the thread structure

	port = tr->last_port_message;
	tr->last_port_message = -1;

	if ( (port >= 0) && (port < MAX_PORTS) )
	{
	  
	  if ( port_flags( proc, port, IPC_MASKED ) != 0 ) return -1;
	  if ( proc->ipc[port].messages == NULL ) return -1;
		// ----- message to return
		
		tmp = proc->ipc[port].messages;			// Remove the first message
		proc->ipc[port].messages = tmp->next;
		
		if ( proc->ipc[port].messages == NULL ) 
			proc->ipc[port].last_message = NULL;
				// Keep the last message
				// correct.
		
		pages = (tmp->bytes + sizeof(struct message)) / PAGE_SIZE + 1;
		free( tmp );
		return 0;
    }


	// ------------ NOW SCAN THROUGH THEM ALL -------------------
	
	for ( i = 0; i < MAX_PORTS; i++ )
	{
		if ( tr->ports[i] == -1 ) break;

		port = tr->ports[i];

		if ( port_flags( proc, port, IPC_MASKED ) != 0 ) continue;
		if ( proc->ipc[port].messages == NULL ) continue;

		// ----- message to return
		
		tmp = proc->ipc[port].messages;			// Remove the first message
		proc->ipc[port].messages = tmp->next;
		
		if ( proc->ipc[port].messages == NULL ) 
			proc->ipc[port].last_message = NULL;
							// Keep the last message
							// correct.
		
		pages = (tmp->bytes + sizeof(struct message)) / PAGE_SIZE + 1;
		free( tmp );
		return 0;
	}
		
	return -1;
}


