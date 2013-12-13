#include <types.h>
#include <kernel.h>
#include <string.h>
#include <stdlib.h>
#include <spoon/base/Lockable.h>
#include <spoon/base/Thread.h>


/// This is the main stub function which is used in all Thread classes. It's job is to call the Thread's Run() method.

/** The __spoon_thread function stub will call the Thread's Run() method 
 * and will set the private variable m_running to false once the Run 
 * method has returned. It is also responsible for the Thread's suicide, 
 * should the Thread be that way inclined.
 *
 * \warning You should never deal with this function. 
 *
 * \param data The default void* parameter passed to system 
 * 		  threads - which happens to be the Thread itself.
 *
 *
 * \return the return value of Thread::Run()
 */
int __spoon_thread( void *data )
{
   if ( data == NULL ) return -1;
		
   Thread *parent = ((Thread*)data);

   	int rc = parent->Run();

	// Mark it as not running.
   parent->lock();
   	  bool killit = parent->m_suicide;
	  parent->m_running = false;
   parent->unlock();

   
   if ( killit == true ) delete parent;
   
   return rc;
}


// ***********************************************************

/** Creates a thread with the name, priority and suicide value as specified. 
 *
 * The thread is created in "suspended" mode and it is up to you to call
 * Resume() on the thread. tid() returns a value thread id once the
 * Thread has been constructed successfully.
 *
 * \warning Kill() the Thread if you instantiate a new thread and delete it 
 * 			without Resuming it. The thread remains active in the system until 
 * 			killed.  
 * 
 *
 * \param name The thread's name.
 * \param priority The thread's priority.
 * \param suicide Specifies whether the thread auto-deletes itself once 
 * 		  completed.
 */
Thread::Thread( const char *name, int priority, bool suicide )
: Lockable() 
{
   m_name = strdup(name);
   m_priority = priority;

   m_suicide 	= suicide;
   m_running 	= false;
   m_terminated = false;

  // Try spawn the thread.
   m_tid = smk_spawn_thread( __spoon_thread, m_name, m_priority, (void*)this );
}


/** The destructor only frees resources allocated for the Thread object 
 * and does not kill the thread. You should look after the Thread's carefully.
 *
 * \warning Only delete finished, Terminated() or Kill()'ed threads. The 
 * 			thread id will remain in the system if you don't clean up properly. 
 */

Thread::~Thread()
{
   free( m_name );
}


// ----------------------------------------------------------

	
/** \return the Thread's name allocated at construction. */
char* Thread::Name() 
{ 
	return m_name; 
}

/** Sets the auto-delete state of the thread. If suicide is true, 
 * then the friend __spoon_thread stub will automatically delete 
 * the Thread object once the Run() method has returned. 
 *
 * \param suicide set to true if you want the thread to auto-delete
 * itself once it's done.
 */
void Thread::SetAutoDelete( bool suicide )
{
   lock();
	m_suicide = suicide;
   unlock();
}



/** Returns true if the Thread is marked as running. The Thread will be 
 * Running() from the time Resume() returns until the time the Run() method 
 * returns. 
 *
 * A Thread will not be Running() before Resume() was called or after the
 * Run() method returns to the __spoon_thread stub.
 * 
 * \return true if the Thread is running.
 */ 
bool Thread::Running()
{
   lock(); 
    bool ans = m_running;
   unlock();
  return ans;
}

/** Returns true if Terminate() was ever called on this Thread.  Run() methods
 * should be implemented to loop or check this value often so that Terminate()
 * behaves in an expected way.
 *
 * \return true if Terminate() has been called on the Thread.
 */
bool Thread::Terminated()
{
   lock(); 
    bool ans = m_terminated;
   unlock();
  return ans;
}


// ***********************************************************


/** Kills a thread. It's theoretically safe to call this on a finished
 * Thread, presuming that you haven't spawned a heck of a lot (like 2GB+)
 * of Threads since the thread finished. This is because tids are 
 * unique within a process.
 *
 * The Thread will stop dead in it's tracks so make sure it's not using
 * anything which could leave a lock locked.
 *
 * \return 0 if the Thread was successfully killed and it sets Running()
 * to false and Terminated() to true. 
 */
int Thread::Kill()
{
  lock();
     int rc = smk_kill_thread( m_tid );
     if ( rc == 0 ) 
     {
       m_running = false;
       m_terminated = true;
     }
  unlock();
  return rc; 
}


/** Sets a spawned thread running. You should always do this once your Thread
 * is set up or else it won't run.
 * 
 *
 * \return 0 if the thread was successfully resumed.  Running() is set to true
 * once this return returns successfully.
 */

int Thread::Resume()
{
  lock();

   // Successfully spawned a thread.
	m_running = true;
  
	  // Resume it.
   	  	int rc = smk_resume_thread( m_tid );
   	  	if ( rc != 0 ) 
	  	{
			  smk_kill_thread( m_tid );	// Kill what we created.
			  m_running = false;
			  rc = -1;
			  m_tid = -1;
	  	}

  unlock();
  return rc; 
}


/** This will set the Terminated() flag to true and the Run() method is
 * supposed to return once this flag is set because it should be
 * checking Terminated() quite often.
 *
 * If wait is true, then Wait() is called with the specified timeout. 
 *
 * \param wait Boolean value indicating whether the method must wait
 * 		  for the Thread to finish. See Wait().
 * \param timeout The timeout value for Wait().
 *
 * \return 0 if wait if false or else it returns the value returned
 * 		   by Wait().
 */
int Thread::Terminate( bool wait, int timeout )
{
	lock();
		m_terminated = true;
	unlock();

	if ( wait == false ) return 0;

	return Wait( timeout );
}


/** Waits for a Thread to finish. This returns if the Thread crashes,
 * finishes or is Kill()'ed.
 *
 * \return return value of the Run() method or -1 if there was some
 * weird error - like the thread had already finished, crashed or a 
 * timeout occured.
 *
 * \param timeout The timeout value in milliseconds to wait for Thread to 
 * 		  finish.
 */
int Thread::Wait( int timeout )
{
  int rc = -1;

  if ( smk_wait_thread( m_tid, &rc ) != 0 ) return -1;

  return rc;
}



/** Returns the thread id of the spawned thread. This is the value
 * returned by the system when the Thread is instantiated. If this
 * value is ever less than 0, then the Thread is not valid.
 *
 * \return thread id.
 */
int Thread::tid() 
{ 
	return m_tid; 
}




