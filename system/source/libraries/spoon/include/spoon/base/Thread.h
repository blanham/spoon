#ifndef _SPOON_BASE_THREAD_H
#define _SPOON_BASE_THREAD_H

/** \ingroup base
 *
 * The Thread class abstracts the system's thread API into a nice, clean C++ API
 * and provides a few operations to manipulate the thread.  Each thread may be named, 
 * have a priority set and be instructed to delete itself once the thread execution 
 * has completed.
 *
 * The main thread is implemented by overriding the pure virtual function Run().
 *
 */


#include <kernel.h>
#include <spoon/ipc/Message.h>
#include <spoon/base/Lockable.h>


class Thread : public Lockable
{
   public: 
     Thread( const char *name, int priority = NORMAL_PRIORITY, bool suicide = false );
     virtual ~Thread();


     int Kill();
     int Resume();
	 int Terminate( bool wait = false, int timeout = INFINITE_TIMEOUT );
	 int Wait( int timeout = INFINITE_TIMEOUT );


     virtual int Run() = 0;		/**< Pure virtual function which must be overriden in child classes. The Run() method should loop until Terminated() returns true. */
	 

     char* Name();
	 void SetAutoDelete( bool suicide = true );
     
	 
	 bool Running();
     bool Terminated();

     int tid();
	 

   private:
	 friend int __spoon_thread(void*); /**<	The function modifies private variables to indicate state */

	 
     char *m_name; 					/**< Thread name cached locally */
     int m_tid;						/**< Thread tid allocated by the system */
	 int m_priority;				/**< Thread priority */

     bool m_running;				/**< Indicates running state */
	 bool m_suicide;				/**< Indicate suicide state */
	 bool m_terminated;				/**< Indicated terminated state */
};

#endif

