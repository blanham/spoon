#include <process.h>
#include <alloc.h>
#include <atomic.h>
#include <misc.h>

#include <paging.h>

#include <ds/htable.h>
#include <rwlock.h>
#include <env.h>


static struct hash_table *l_processTable = NULL;	//< Table for all pids
static struct process 	 *l_rootProcess  = NULL;	//< Root process
static struct rwlock 	 *l_processLock  = NULL;	//< RW lock for processes


static int l_PID = 1;						//< PID counter.
static unsigned int l_PIDlock	=	0;		//< PID lock.



// -------------------------    hash table key and compare functions.

int ptable_key( void *d )
{
	struct process *p = (struct process*)d;
	int ans = p->pid * 100;
	if ( ans < 0 ) ans = -ans;
	return ans;
}

int ptable_compare( void* ad, void* bd )
{
	struct process *a = (struct process*)ad;
	struct process *b = (struct process*)bd;

	if ( (b->pid != -1) )
	{
		if ( a->pid == b->pid ) return 0; 
		if ( a->pid < b->pid ) return -1;
		return 1;
	}
	
	return strncmp( a->name, b->name, NAME_LENGTH );
}


// --------------------------------------------------------


int next_pid()
{
	int pid;
	struct process temp;
				   temp.name[0] = 0;
				   temp.pid = -1;

	acquire_spinlock( &(l_PIDlock) );
		
		do
		{
			if ( temp.pid != -1 )
			{
				l_PID += 1;
				if ( l_PID < 0 ) l_PID = 2;
			}
	
			temp.pid = l_PID;
		}
		while ( htable_retrieve( l_processTable, &temp ) != NULL );

		pid = l_PID++;

	release_spinlock( &(l_PIDlock) );

	return pid;
}


// --------------------------------------------------------

void init_processes()
{
	l_processTable = init_htable( 128, 75, ptable_key, ptable_compare );
	l_processLock = init_rwlock();

	assert( l_PID == 1 );
}



struct process *new_process( const char name[NAME_LENGTH] )
{
	struct process *proc = malloc( sizeof(struct process) );
	if ( proc == NULL ) return NULL;

	strncpy( proc->name, name, NAME_LENGTH );

#warning "protential for problems if more than MAXINT processes are spawned at once"
	proc->pid = next_pid();
	proc->state = PROCESS_INIT;

	proc->map = new_page_directory();
	proc->lock = init_rwlock();

	proc->threads = NULL;
	proc->threads_htable = init_htable( 128, 75, threads_key, threads_compare );   
	proc->thread_count = 0;

	proc->last_tid = 1;

	proc->kernel_threads = 0;

	proc->flags = 0;

	proc->environment = NULL;
	proc->environment_count = 0;

	proc->alloc = NULL;



	FAMILY_INIT( proc );
	LIST_INIT( proc );
	return proc;
}


void del_process( struct process* proc )
{
	delete_page_directory( proc->map );
	delete_rwlock( proc->lock );
	release_environment( proc );

#warning release ualloc regions
#warning MAINTAIN ACCURATELY

	free( proc );
}




int insert_process( int pid, struct process *proc )
{
	struct process *parent = NULL;
	struct process temp;

	if ( l_rootProcess == NULL )
	{
		l_rootProcess = proc;
		htable_insert( l_processTable, proc );
		return proc->pid;		
	}

	assert( pid > 0 );

	// Get the parent process.
		temp.name[0] = 0;
		temp.pid = pid;
	
	
	rwlock_get_write_access( l_processLock );		// We are writing the process list.
	
		parent = (struct process*)htable_retrieve( l_processTable, &temp );
		if ( parent == NULL ) return -1;

	
		// Insertion.
		family_add_child( &(parent->family_tree), &(proc->family_tree) );
		list_insertAfter( &(l_rootProcess->list), &(proc->list) );
		htable_insert( l_processTable, proc );

		temp.pid = proc->pid;

	rwlock_release( l_processLock );

	return temp.pid;
}

void remove_process( struct process *proc )
{
	rwlock_get_write_access( l_processLock );		// We are writing the process list.

#warning FAMILIAL SAVE

		family_remove( &(proc->family_tree) );
		list_remove( &(proc->list) );
		htable_remove( l_processTable, proc );
	  
	rwlock_release( l_processLock );
}



// --------------------------------------------------------------


/** Acquires a rwlock on the global process list and on the
	process requested, if found. 
	
	flags = READER | WRITER
*/

struct process *checkout_process( int pid,  unsigned int flags )
{
	struct process temp;
	struct process *p;

		  temp.pid = pid;
		  temp.name[0] = 0;

	rwlock_get_read_access( l_processLock );		// We are reading.

		p = ((struct process*)htable_retrieve( l_processTable, &temp ));

		if ( p == NULL )
		{
			rwlock_release( l_processLock );
			return NULL;
		}

	if ( (flags == READER) ) rwlock_get_read_access( p->lock );
						else rwlock_get_write_access( p->lock );
	
	// Leave the global process list locked to ensure data integrity.
	
	return p;
}


/** Release a process and the global process list.  */

void 			commit_process( struct process *p )
{
	rwlock_release( p->lock );
	rwlock_release( l_processLock );
}




